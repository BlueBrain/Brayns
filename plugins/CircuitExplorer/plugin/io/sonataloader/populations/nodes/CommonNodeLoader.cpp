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

#include "CommonNodeLoader.h"

#include <plugin/io/morphology/neuron/NeuronBuilder.h>
#include <plugin/io/morphology/neuron/NeuronMorphologyPipeline.h>
#include <plugin/io/sonataloader/data/SonataConfig.h>

#include <future>

namespace sonataloader
{
std::vector<MorphologyInstance::Ptr> CommonNodeLoader::loadNodes(
    const SonataConfig::Data& networkData,
    const SonataNodePopulationParameters& lc,
    const bbp::sonata::Selection& nodeSelection,
    const std::vector<std::string>& morphologyNames,
    const std::vector<brayns::Vector3f>& positions,
    const std::vector<brayns::Quaternion>& rotations) const
{
    const auto nodesSize = nodeSelection.flatSize();

    std::vector<MorphologyInstance::Ptr> result(nodesSize);

    // Maps morphology class to all cell indices that uses that class
    std::unordered_map<std::string, std::vector<size_t>> morphologyMap;
    for (size_t i = 0; i < nodesSize; ++i)
        morphologyMap[morphologyNames[i]].push_back(i);

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

    const auto morphPath = SonataConfig::resolveMorphologyPath(
        networkData.config.getNodePopulationProperties(lc.node_population));

    const auto loadFn = [&](const std::string& name,
                            const std::vector<size_t>& indices) {
        const auto path = morphPath.buildPath(name);
        NeuronMorphology morphology(path, loadSoma, loadAxon, loadDend);
        pipeline.process(morphology);
        const auto instantiable = builder.build(morphology);

        for (const auto idx : indices)
            result[idx] =
                instantiable->instantiate(positions[idx], rotations[idx]);
    };

    // Use system threadpools
    std::vector<std::future<void>> loadTasks;
    loadTasks.reserve(morphologyMap.size());
    for (const auto& entry : morphologyMap)
        loadTasks.push_back(std::async(loadFn, entry.first, entry.second));

    for (const auto& task : loadTasks)
    {
        if (task.valid())
            task.wait();
    }

    return result;
}
} // namespace sonataloader
