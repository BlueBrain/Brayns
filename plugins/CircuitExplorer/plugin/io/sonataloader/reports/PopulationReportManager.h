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

#include <brayns/engine/Model.h>

#include <plugin/io/SonataLoaderParameters.h>
#include <plugin/io/morphology/MorphologyInstance.h>
#include <plugin/io/sonataloader/reports/EdgeReportLoader.h>
#include <plugin/io/sonataloader/reports/NodeReportLoader.h>
#include <plugin/io/synapse/SynapseGroup.h>

namespace sonataloader
{
class PopulationReportManager
{
public:
    /**
     * @brief if the given population was requested to be loaded with a report,
     * it loads and sets the geometry simulation mapping for the given nodes of
     * the node population
     */
    static void loadNodeMapping(const SonataNodePopulationParameters& input,
                                const bbp::sonata::Selection& selection,
                                std::vector<MorphologyInstance::Ptr>& nodes);

    /**
     * @brief if the given population was requested to be loaded with a report,
     * it loads and sets the geometry simulation mapping for the given edges of
     * the edge population
     */
    static void loadEdgeMapping(const SonataEdgePopulationParameters& input,
                                const bbp::sonata::Selection& selection,
                                std::vector<SynapseGroup::Ptr>& edges);

    /**
     * @brief if the given population was requested to be loaded with a report,
     * it instantiates the appropriate simulation handler, enabled simulation
     * coloring and handles any special case (such as vasculature radii reports)
     */
    static void addNodeReportHandler(
        const SonataNodePopulationParameters& input,
        const bbp::sonata::Selection& selection,
        brayns::ModelDescriptorPtr& model);

    /**
     * @brief if the given population was requested to be loaded with a report,
     * it instantiates the appropriate simulation handler and enabled simulation
     */
    static void addEdgeReportHandler(
        const SonataEdgePopulationParameters& input,
        const bbp::sonata::Selection& selection,
        brayns::ModelDescriptorPtr& model);

    /**
     * @brief Checks each edge population. If they dont have a simulation of
     * their own, and the node model has, it sets that simulation to the edges
     * as well
     */
    static void addNodeHandlerToEdges(
        const brayns::ModelDescriptorPtr& nodeModel,
        const std::vector<brayns::ModelDescriptor*>& edgeModels);
};
} // namespace sonataloader
