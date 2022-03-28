/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include "CellLoader.h"

#include <plugin/components/CircuitColorComponent.h>
#include <plugin/io/morphology/neuron/NeuronMorphologyPipeline.h>
#include <plugin/io/morphology/neuron/builders/PrimitiveNeuronBuilder.h>
#include <plugin/io/morphology/neuron/colorhandlers/PrimitiveColorHandler.h>
#include <plugin/io/morphology/neuron/colorhandlers/SomaColorHandler.h>
#include <plugin/io/morphology/neuron/components/MorphologyCircuitComponent.h>
#include <plugin/io/morphology/neuron/components/SomaCircuitComponent.h>
#include <plugin/io/bbploader/colordata/BBPNeuronColorData.h>

#include <brion/blueConfig.h>

#include <future>
#include <filesystem>

namespace
{
struct BBPNeuronColorCreator
{
    static std::unique_ptr<bbploader::BBPNeuronColorData> newData(const brion::BlueConfig& config,
                                                                  const brain::Circuit &circuit)
    {
        auto circuitPath = circuit.getSource().getPath();
        auto circuitPopulation = config.getCircuitPopulation();
        return std::make_unique<bbploader::BBPNeuronColorData>(std::move(circuitPath), std::move(circuitPopulation));
    }
};
}

namespace bbploader
{
std::vector<CompartmentStructure> CellLoader::load(const BBPLoaderParameters &lc,
                                                   const std::vector<uint64_t> &ids,
                                                   const brain::Circuit &circuit,
                                                   const brion::BlueConfig &blueConfig,
                                                   ProgressUpdater &updater,
                                                   brayns::Model &model)
{
    const auto &morphSettings = lc.neuron_morphology_parameters;
    const auto loadSoma = morphSettings.load_soma;
    const auto loadAxon = morphSettings.load_axon;
    const auto loadDend = morphSettings.load_dendrites;
    const auto onlySoma = loadSoma && !loadAxon && !loadDend;
    const auto radMultiplier = morphSettings.radius_multiplier;
    const auto radOverride = morphSettings.radius_override;

    const brain::GIDSet gids (ids.begin(), ids.end());

    const auto positions = circuit.getPositions(gids);

    std::vector<CompartmentStructure> result(gids.size());

    auto colorData = BBPNeuronColorCreator::newData(blueConfig, circuit);
    std::unique_ptr<CircuitColorHandler> colorHandler;

    if(onlySoma)
    {
        auto &somaCircuit = model.addComponent<SomaCircuitComponent>();
        colorHandler = std::make_unique<SomaColorHandler>(*colorData, somaCircuit);

        const auto radius = radOverride > 0.f? radOverride : radMultiplier;
#pragma omp parallel for
        for(size_t i = 0; i < ids.size(); ++i)
        {
            const auto id = ids[i];
            const auto &pos = positions[i];
            brayns::Sphere somaGeometry = {pos, radius};
            somaCircuit.addSoma(id, std::move(somaGeometry));

            auto &compartments = result[i];
            compartments.sectionSegments[-1].push_back(0);
        }
    }
    else
    {
        auto &morphologyCircuit = model.addComponent<MorphologyCircuitComponent>();
        colorHandler = std::make_unique<PrimitiveColorHandler>(*colorData, morphologyCircuit);

        const auto morphPaths = circuit.getMorphologyURIs(gids);
        const auto rotations = circuit.getRotations(gids);

        // Group indices by the morphology name, so we will load the morphology
        // once, and then iterate over the indices of the corresponding cells
        std::unordered_map<std::string, std::vector<size_t>> morphPathMap;
        for (size_t i = 0; i < morphPaths.size(); ++i)
        {
            morphPathMap[morphPaths[i].getPath()].push_back(i);
        }

        const auto pipeline = NeuronMorphologyPipeline::create(radMultiplier, radOverride);
        std::vector<PrimitiveNeuronGeometry> morphologies (ids.size());
        const auto loadFn = [&](const std::string &path, const std::vector<size_t> &indices)
        {
            NeuronMorphology morphology(path, loadSoma, loadAxon, loadDend);
            pipeline.process(morphology);
            const PrimitiveNeuronBuilder builder(morphology);
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

        const std::string updateMessage ("Loading neurons");
        for (auto &task : loadTasks)
        {
            if (task.valid())
            {
                task.get();
                updater.update(updateMessage);
            }
            else
            {
                throw std::runtime_error("Unknown error while loading morphologies");
            }
        }

#pragma omp parallel for
        for(size_t i = 0; i < ids.size(); ++i)
        {
            const auto id = ids[i];
            auto &morphology = morphologies[i];
            auto &sectionMapping = morphology.sectionMapping;
            auto &compartmentMapping = morphology.sectionSegmentMapping;
            auto &geometry = morphology.geometry;

            result[i].sectionSegments = std::move(compartmentMapping);

            morphologyCircuit.addMorphology(id, std::move(geometry),std::move(sectionMapping));
        }
    }

    model.addComponent<CircuitColorComponent>(std::move(colorData), std::move(colorHandler));

    return result;
}
} // namespace bbploader
