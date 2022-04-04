#include "MorphologyCircuitLoader.h"

#include <components/CircuitColorComponent.h>
#include <io/circuit/colorhandlers/MorphologyColorHandler.h>
#include <io/circuit/components/MorphologyCircuitComponent.h>
#include <io/morphology/neuron/NeuronGeometryBuilder.h>
#include <io/morphology/neuron/NeuronMorphologyProcessor.h>

#include <future>
#include <unordered_map>

MorphologyCircuitLoader::Context::Context(
    const std::vector<uint64_t> &ids,
    const std::vector<std::string> &morphologyPaths,
    const std::vector<brayns::Vector3f> &positions,
    const std::vector<brayns::Quaternion> &rotations,
    const NeuronMorphologyLoaderParameters &morphologyParams)
    : ids(ids)
    , morphologyPaths(morphologyPaths)
    , positions(positions)
    , rotations(rotations)
    , morphologyParams(morphologyParams)
{
}

std::vector<CompartmentStructure> MorphologyCircuitLoader::load(
    const Context &context,
    brayns::Model &model,
    ProgressUpdater &cb,
    std::unique_ptr<IColorData> colorData)
{
    const auto &morphPaths = context.morphologyPaths;
    const auto &ids = context.ids;
    const auto &morphParams = context.morphologyParams;
    const auto soma = morphParams.load_soma;
    const auto axon = morphParams.load_axon;
    const auto dendrites = morphParams.load_dendrites;
    const auto radiusMultiplier = morphParams.radius_multiplier;
    const auto &positions = context.positions;
    const auto &rotations = context.rotations;

    // Group indices by the morphology name, so we will load the morphology
    // once, and then iterate over the indices of the corresponding cells
    std::unordered_map<std::string, std::vector<size_t>> morphPathMap;
    for (size_t i = 0; i < morphPaths.size(); ++i)
    {
        morphPathMap[morphPaths[i]].push_back(i);
    }

    std::vector<NeuronGeometry> morphologies(ids.size());
    const auto loadFn = [&](const std::string &path, const std::vector<size_t> &indices)
    {
        NeuronMorphology morphology(path, soma, axon, dendrites);
        NeuronMorphologyProcessor::processMorphology(morphology, radiusMultiplier);
        const NeuronGeometryBuilder builder(morphology);
        for (const auto idx : indices)
        {
            morphologies[idx] = builder.instantiate(positions[idx], rotations[idx]);
        }
    };

    std::vector<std::future<void>> loadTasks;
    loadTasks.reserve(morphPathMap.size());
    for (const auto &entry : morphPathMap)
    {
        loadTasks.push_back(std::async(loadFn, entry.first, entry.second));
    }

    const std::string updateMessage("Loading neurons");
    for (auto &task : loadTasks)
    {
        if (task.valid())
        {
            task.get();
            cb.update(updateMessage);
        }
        else
        {
            throw std::runtime_error("Unknown error while loading morphologies");
        }
    }

    std::vector<CompartmentStructure> compartments(ids.size());
    std::vector<std::vector<brayns::Primitive>> geometries(ids.size());
    std::vector<std::vector<NeuronSectionMapping>> mappings(ids.size());

#pragma omp parallel for
    for (size_t i = 0; i < ids.size(); ++i)
    {
        auto &morphology = morphologies[i];
        auto &sectionMapping = morphology.sectionMapping;
        auto &compartmentMapping = morphology.sectionSegmentMapping;
        auto &geometry = morphology.geometry;

        mappings[i] = std::move(sectionMapping);

        auto &compartment = compartments[i];
        compartment.id = ids[i];
        compartment.numItems = geometry.size();
        compartment.sectionSegments = std::move(compartmentMapping);

        geometries[i] = std::move(geometry);
    }

    auto &morphologyCircuit = model.addComponent<MorphologyCircuitComponent>();
    morphologyCircuit.setMorphologies(ids, std::move(geometries), std::move(mappings));

    auto colorHandler = std::make_unique<MorphologyColorHandler>(morphologyCircuit);
    model.addComponent<CircuitColorComponent>(std::move(colorData), std::move(colorHandler));

    return compartments;
}
