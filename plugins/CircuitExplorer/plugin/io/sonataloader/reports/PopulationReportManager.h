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

#pragma once

#include <brayns/common/simulation/AbstractSimulationHandler.h>

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
    static void loadNodeMapping(const SonataNodePopulationParameters& input,
                                const bbp::sonata::Selection& selection,
                                std::vector<MorphologyInstance::Ptr>& nodes);
    static void loadEdgeMapping(const SonataEdgePopulationParameters& input,
                                const bbp::sonata::Selection& selection,
                                std::vector<SynapseGroup::Ptr>& edges);

    static void addNodeReportHandler(const SonataNodePopulationParameters& input,
                                     const bbp::sonata::Selection& selection,
                                     brayns::ModelDescriptorPtr& model);
    static void addEdgeReportHandler(const SonataEdgePopulationParameters& input,
                                     const bbp::sonata::Selection& selection,
                                     brayns::ModelDescriptorPtr& model);
};
}
