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

#include "SonataSimulationMapping.h"

#include <highfive/H5File.hpp>

#include <unordered_map>
#include <vector>

namespace
{
using Range = std::pair<uint64_t, uint64_t>;

auto computeMapping(
    const HighFive::Group &reportPop,
    const std::unordered_map<bbp::sonata::NodeID, Range> &nodePointers,
    const std::vector<bbp::sonata::NodeID> &nodeIds)
{
    std::vector<std::pair<bbp::sonata::NodeID, bbp::sonata::ElementID>> result;
    auto elementIdsData = reportPop.getGroup("mapping").getDataSet("element_ids");
    for (const auto nodeId : nodeIds)
    {
        const auto it = nodePointers.find(nodeId);
        if (it == nodePointers.end())
        {
            continue;
        }

        std::vector<bbp::sonata::ElementID> elementIds(it->second.second - it->second.first);
        elementIdsData.select({it->second.first}, {it->second.second - it->second.first}).read(elementIds.data());

        for (const auto elem : elementIds)
        {
            result.push_back(std::make_pair(nodeId, elem));
        }
    }
    return result;
}
} // namespace

namespace sonataloader
{
std::vector<bbp::sonata::NodeID> SonataSimulationMapping::getCompartmentNodes(
    const std::string &reportPath,
    const std::string &population)
{
    const HighFive::File file(reportPath, HighFive::File::ReadOnly);
    const auto reportPop = file.getGroup("/report/" + population);

    std::vector<bbp::sonata::NodeID> reportNodeIds;
    const auto mappingGroup = reportPop.getGroup("mapping");
    mappingGroup.getDataSet("node_ids").read(reportNodeIds);
    return reportNodeIds;
}

// Extracted and optimized code
// from https://github.com/BlueBrain/libsonata/blob/master/src/report_reader.cpp
std::vector<std::pair<bbp::sonata::NodeID, bbp::sonata::ElementID>> SonataSimulationMapping::getCompartmentMapping(
    const std::string &reportPath,
    const std::string &population,
    const std::vector<bbp::sonata::NodeID> &nodeIds)
{
    // Get report population
    const HighFive::File file(reportPath, HighFive::File::ReadOnly);
    const auto reportPop = file.getGroup("/report/" + population);

    // Get reported node Ids
    std::vector<bbp::sonata::NodeID> reportNodeIds;
    const auto mappingGroup = reportPop.getGroup("mapping");
    mappingGroup.getDataSet("node_ids").read(reportNodeIds);

    // Get indices
    std::vector<uint64_t> indexPointers;
    mappingGroup.getDataSet("index_pointers").read(indexPointers);

    // Create node pointers to know how to access the mapping dataset

    std::unordered_map<bbp::sonata::NodeID, Range> nodePointers;
    for (size_t i = 0; i < reportNodeIds.size(); ++i)
    {
        const auto nodeId = reportNodeIds[i];
        const auto start = indexPointers[i];
        const auto end = indexPointers[i + 1];

        nodePointers.emplace(nodeId, std::make_pair(start, end));
    }

    // Compute final list of
    if (!nodeIds.empty())
    {
        return computeMapping(reportPop, nodePointers, nodeIds);
    }

    return computeMapping(reportPop, nodePointers, reportNodeIds);
}
} // namespace sonataloader
