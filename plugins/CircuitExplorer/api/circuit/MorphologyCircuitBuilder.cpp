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

#include <future>
#include <unordered_map>

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
        auto morphology = NeuronMorphologyReader::read(path, soma, axon, dendrites);
        NeuronMorphologyProcessor::processMorphology(morphology, true, radiusMultiplier);
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
