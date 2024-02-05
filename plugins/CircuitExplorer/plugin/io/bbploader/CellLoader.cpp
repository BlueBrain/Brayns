/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <plugin/io/morphology/neuron/NeuronBuilder.h>
#include <plugin/io/morphology/neuron/NeuronMorphologyPipeline.h>

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

    const auto& morphSettings = lc.neuron_morphology_parameters;
    const auto& geometryMode = morphSettings.geometry_mode;
    const auto radMultiplier = morphSettings.radius_multiplier;
    const auto radOverride = morphSettings.radius_override;
    const auto loadSoma = morphSettings.load_soma;
    const auto loadAxon = morphSettings.load_axon;
    const auto loadDend = morphSettings.load_dendrites;

    const NeuronBuilderTable builderTable;
    const auto& builder = builderTable.getBuilder(geometryMode);
    const NeuronMorphologyPipeline pipeline =
        NeuronMorphologyPipeline::create(radMultiplier, radOverride,
                                         geometryMode == "smooth" &&
                                             (loadAxon || loadDend));

    std::vector<MorphologyInstance::Ptr> cells(gids.size());

    const auto loadFn = [&](const std::string& path,
                            const std::vector<size_t>& indices) {
        NeuronMorphology morphology(path, loadSoma, loadAxon, loadDend);
        pipeline.process(morphology);
        const auto instantiable = builder.build(morphology);
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
