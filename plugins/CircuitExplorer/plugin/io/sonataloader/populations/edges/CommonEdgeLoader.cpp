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

#include "CommonEdgeLoader.h"

#include <plugin/io/sonataloader/data/SonataSelection.h>
#include <plugin/io/sonataloader/data/SonataSynapses.h>
#include <plugin/io/synapse/groups/SurfaceSynapseGroup.h>

namespace sonataloader
{
std::vector<std::unique_ptr<SynapseGroup>> CommonEdgeLoader::load(
    const SonataConfig::Data& networkConfig,
    const SonataEdgePopulationParameters& lc,
    const bbp::sonata::Selection& nodeSelection) const
{
    const auto baseNodeList = nodeSelection.flatten();

    // Fill it by mapping node ID to synapse list in case there is a node Id
    // without synapses, so we can still place an empty vector at the end
    std::map<uint64_t, std::unique_ptr<SynapseGroup>> mapping;
    for (const auto nodeId : baseNodeList)
        mapping[nodeId] = std::make_unique<SurfaceSynapseGroup>();

    std::vector<uint64_t> srcNodes;
    std::vector<int32_t> sectionIds;
    std::vector<float> distances;
    std::vector<brayns::Vector3f> surfacePos;
    std::vector<uint64_t> edgeIds;

    const auto population =
        networkConfig.config.getEdgePopulation(lc.edge_population);

    if (lc.load_afferent)
    {
        const auto edgeSelection =
            EdgeSelection(population.afferentEdges(baseNodeList))
                .intersection(lc.edge_percentage);
        edgeIds = edgeSelection.flatten();
        srcNodes = SonataSynapses::getTargetNodes(population, edgeSelection);
        sectionIds =
            SonataSynapses::getAfferentSectionIds(population, edgeSelection);
        surfacePos =
            SonataSynapses::getAfferentSurfacePos(population, edgeSelection);
        distances = SonataSynapses::getAfferentSectionDistances(population,
                                                                edgeSelection);
    }
    else
    {
        const auto edgeSelection =
            EdgeSelection(population.efferentEdges(baseNodeList))
                .intersection(lc.edge_percentage);
        edgeIds = edgeSelection.flatten();
        srcNodes = SonataSynapses::getSourceNodes(population, edgeSelection);
        surfacePos =
            SonataSynapses::getEfferentSurfacePos(population, edgeSelection);
        sectionIds =
            SonataSynapses::getEfferentSectionIds(population, edgeSelection);
        distances = SonataSynapses::getEfferentSectionDistances(population,
                                                                edgeSelection);
    }

    if (srcNodes.size() != sectionIds.size() ||
        srcNodes.size() != surfacePos.size() ||
        srcNodes.size() != distances.size())
        throw std::runtime_error("Edge population '" + population.name() +
                                 "' attributes missmatch in size");

    // Group data by node id
    for (size_t i = 0; i < srcNodes.size(); ++i)
    {
        auto& buffer =
            static_cast<SurfaceSynapseGroup&>(*mapping[srcNodes[i]].get());
        buffer.addSynapse(edgeIds[i], sectionIds[i], distances[i],
                          surfacePos[i]);
    }

    // Flatten
    std::vector<std::unique_ptr<SynapseGroup>> synapses(baseNodeList.size());
    for (size_t i = 0; i < baseNodeList.size(); ++i)
    {
        auto it = mapping.find(baseNodeList[i]);
        if (it != mapping.end())
            synapses[i] = std::move(it->second);
    }

    return synapses;
}
} // namespace sonataloader
