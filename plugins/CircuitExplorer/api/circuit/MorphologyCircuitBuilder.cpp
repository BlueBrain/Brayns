/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "MorphologyCircuitBuilder.h"

#include "colorhandlers/MorphologyColorHandler.h"

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryCommitSystem.h>
#include <brayns/engine/systems/GeometryInitSystem.h>

#include <api/neuron/NeuronGeometryBuilder.h>
#include <api/neuron/NeuronMorphologyPipeline.h>
#include <api/neuron/NeuronMorphologyReader.h>
#include <components/CircuitIds.h>
#include <components/Coloring.h>
#include <components/NeuronSectionList.h>
#include <systems/NeuronInspectSystem.h>

#include <deque>
#include <future>
#include <unordered_map>

namespace
{
struct MorphologyMap
{
    size_t cellCount{};
    std::unordered_map<std::string, std::vector<size_t>> pathToCellIndices;
};

class MorphologyMapBuilder
{
public:
    static MorphologyMap build(const std::vector<std::string> &paths)
    {
        MorphologyMap result;
        result.cellCount = paths.size();

        auto &mapping = result.pathToCellIndices;

        for (size_t i = 0; i < paths.size(); ++i)
        {
            mapping[paths[i]].push_back(i);
        }

        return result;
    }
};
class ParallelMorphologyLoader
{
public:
    static std::vector<NeuronGeometry> load(
        const MorphologyMap &morphologyMap,
        const NeuronMorphologyLoaderParameters &morphologyParameters,
        const std::vector<brayns::Vector3f> &positions,
        const std::vector<brayns::Quaternion> &rotations,
        ProgressUpdater &progressUpdater)
    {
        const auto cellCount = morphologyMap.cellCount;

        std::vector<NeuronGeometry> morphologies(cellCount);

        const auto soma = morphologyParameters.load_soma;
        const auto axon = morphologyParameters.load_axon;
        const auto dendrites = morphologyParameters.load_dendrites;
        const auto radiusMultiplier = morphologyParameters.radius_multiplier;
        const auto geometryType = morphologyParameters.geometry_type;
        const auto pipeline = NeuronMorphologyPipeline::fromParameters(geometryType, radiusMultiplier);

        const auto loadFn = [&](const std::string &path, const std::vector<size_t> &indices)
        {
            auto morphology = NeuronMorphologyReader::read(path, soma, axon, dendrites);
            pipeline.process(morphology);
            const NeuronGeometryBuilder builder(morphology);
            for (const auto idx : indices)
            {
                morphologies[idx] = builder.instantiate(positions[idx], rotations[idx]);
            }
        };

        const auto updateMessage = std::string("Loading neurons");
        const auto &pathToCellIndexMap = morphologyMap.pathToCellIndices;
        constexpr size_t maxThreads = 800;

        std::deque<std::future<void>> loadTasks;
        for (const auto &[path, cellIndices] : pathToCellIndexMap)
        {
            if (loadTasks.size() == maxThreads)
            {
                auto &topTask = loadTasks.front();
                topTask.get();
                loadTasks.pop_front();
                progressUpdater.update(updateMessage);
            }

            loadTasks.push_back(std::async(loadFn, path, cellIndices));
        }

        for (auto &task : loadTasks)
        {
            task.get();
            progressUpdater.update(updateMessage);
        }

        return morphologies;
    }
};

class ModelBuilder
{
public:
    ModelBuilder(brayns::Model &model)
        : _model(model)
    {
    }

    void addIds(const std::vector<uint64_t> &ids)
    {
        _model.getComponents().add<CircuitIds>(ids);
    }

    void addGeometry(std::vector<std::vector<brayns::Capsule>> primitiveList)
    {
        auto &components = _model.getComponents();
        auto &geometries = components.add<brayns::Geometries>();
        geometries.elements.reserve(primitiveList.size());
        for (auto &primitives : primitiveList)
        {
            geometries.elements.emplace_back(std::move(primitives));
        }
    }

    void addNeuronSections(std::vector<std::vector<NeuronSectionMapping>> sections)
    {
        _model.getComponents().add<NeuronSectionList>(std::move(sections));
    }

    void addColoring(std::unique_ptr<IColorData> data)
    {
        auto &components = _model.getComponents();
        auto handler = std::make_unique<MorphologyColorHandler>(components);
        components.add<Coloring>(std::move(data), std::move(handler));
    }

    void addSystems()
    {
        auto &systems = _model.getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        systems.setInitSystem<brayns::GeometryInitSystem>();
        systems.setCommitSystem<brayns::GeometryCommitSystem>();
        systems.setInspectSystem<MorphologyInspectSystem>();
    }

private:
    brayns::Model &_model;
};
}

MorphologyCircuitBuilder::Context::Context(
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

std::vector<CellCompartments> MorphologyCircuitBuilder::load(
    const Context &context,
    brayns::Model &model,
    ProgressUpdater &updater,
    std::unique_ptr<IColorData> colorData)
{
    auto &morphPaths = context.morphologyPaths;
    auto &ids = context.ids;
    auto &morphParams = context.morphologyParams;
    auto &positions = context.positions;
    auto &rotations = context.rotations;

    auto morphologyPathMap = MorphologyMapBuilder::build(morphPaths);
    auto morphologies = ParallelMorphologyLoader::load(morphologyPathMap, morphParams, positions, rotations, updater);

    std::vector<CellCompartments> compartments(ids.size());
    std::vector<std::vector<brayns::Capsule>> geometries(ids.size());
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
        compartment.numItems = geometry.size();
        compartment.sectionSegments = std::move(compartmentMapping);

        geometries[i] = std::move(geometry);
    }

    auto builder = ModelBuilder(model);
    builder.addIds(ids);
    builder.addGeometry(std::move(geometries));
    builder.addNeuronSections(std::move(mappings));
    builder.addColoring(std::move(colorData));
    builder.addSystems();

    return compartments;
}
