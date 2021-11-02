/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include <plugin/io/morphology/neuron/NeuronMorphologyImporter.h>

#include <future>

namespace bbploader
{
std::vector<MorphologyInstance::Ptr> CellLoader::load(
    const BBPLoaderParameters& lc, const brain::GIDSet& gids,
    const brain::Circuit& circuit)
{
    const auto morphPaths = circuit.getMorphologyURIs(gids);
    // Group indices by the morphology name, so we will load the morphology
    // once, and then iterate over the indices of the corresponding cells
    std::unordered_map<std::string, std::vector<size_t>> morphPathMap;
    for (size_t i = 0; i < morphPaths.size(); ++i)
        morphPathMap[morphPaths[i].getPath()].push_back(i);

    const auto positions = circuit.getPositions(gids);
    const auto rotations = circuit.getRotations(gids);

    NeuronMorphologyImporter::ImportSettings config;
    config.radiusMultiplier = lc.neuron_morphology_parameters.radius_multiplier;
    config.radiusOverride = lc.neuron_morphology_parameters.radius_override;
    config.builderName = lc.neuron_morphology_parameters.geometry_mode;
    config.loadAxon = lc.neuron_morphology_parameters.load_axon;
    config.loadDendrites = lc.neuron_morphology_parameters.load_dendrites;
    config.loadSoma = lc.neuron_morphology_parameters.load_soma;
    const NeuronMorphologyImporter importer(config);

    std::vector<MorphologyInstance::Ptr> cells(gids.size());

    const auto loadFn = [&](const std::string& path,
                            const std::vector<size_t>& indices) {
        const auto instantiable = importer.import(path);
        for (const auto idx : indices)
            cells[idx] =
                instantiable->instantiate(positions[idx], rotations[idx]);
    };

    std::vector<std::future<void>> loadTasks;
    loadTasks.reserve(morphPathMap.size());
    for (const auto& entry : morphPathMap)
        loadTasks.push_back(std::async(loadFn, entry.first, entry.second));

    for (const auto& task : loadTasks)
    {
        if (task.valid())
            task.wait();
    }

    return cells;
}
} // namespace bbploader
