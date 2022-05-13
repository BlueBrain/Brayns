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

#include <api/neuron/NeuronGeometryBuilder.h>
#include <api/neuron/NeuronMorphologyProcessor.h>
#include <api/neuron/NeuronMorphologyReader.h>
#include <components/CircuitColorComponent.h>
#include <components/MorphologyCircuitComponent.h>

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

        const auto loadFn = [&](const std::string &path, const std::vector<size_t> &indices)
        {
            auto morphology = NeuronMorphologyReader::read(path, soma, axon, dendrites);
            NeuronMorphologyProcessor::processMorphology(morphology, true, radiusMultiplier);
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
    const auto &morphPaths = context.morphologyPaths;
    const auto &ids = context.ids;
    const auto &morphParams = context.morphologyParams;
    const auto &positions = context.positions;
    const auto &rotations = context.rotations;

    const auto morphologyPathMap = MorphologyMapBuilder::build(morphPaths);
    auto morphologies = ParallelMorphologyLoader::load(morphologyPathMap, morphParams, positions, rotations, updater);

    std::vector<CellCompartments> compartments(ids.size());
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
