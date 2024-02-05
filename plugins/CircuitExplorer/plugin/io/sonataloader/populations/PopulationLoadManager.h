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

#pragma once

#include <plugin/io/sonataloader/populations/EdgePopulationLoader.h>
#include <plugin/io/sonataloader/populations/NodePopulationLoader.h>

namespace sonataloader
{
/**
 * @brief The PopulationLoaderManager class gives access to all the edge and
 * node population loaders organized by the population type
 */
class PopulationLoaderManager
{
public:
    /**
     * @brief returns a reference to the edge loader object that can handle the
     * type of edge population denoted by edgeType
     */
    static std::vector<SynapseGroup::Ptr> loadEdges(
        const SonataConfig::Data& networkConfig,
        const SonataEdgePopulationParameters& lc,
        const bbp::sonata::Selection& nodeSelection);

    /**
     * @brief returns a reference to the node loader object that can handle the
     * type of node population denoted by nodeType
     */
    static std::vector<MorphologyInstance::Ptr> loadNodes(
        const SonataConfig::Data& networkData,
        const SonataNodePopulationParameters& loadSettings,
        const bbp::sonata::Selection& nodeSelection);

    /**
     * @brief Adjusts the geometry of the edges to the geometry of the nodes
     * they belong to. It also sets the simulation mapping of the edge to that
     * of the nodes.
     */
    static void mapEdgesToNodes(
        const std::vector<MorphologyInstance::Ptr>& nodes,
        std::vector<SynapseGroup::Ptr>& edges);
};
} // namespace sonataloader
