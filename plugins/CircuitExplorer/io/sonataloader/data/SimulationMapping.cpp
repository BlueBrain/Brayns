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

#include "SimulationMapping.h"

#include <highfive/H5File.hpp>

#include <unordered_map>
#include <vector>

namespace
{
using Range = std::pair<uint64_t, uint64_t>;
using Mapping = std::pair<bbp::sonata::NodeID, bbp::sonata::ElementID>;

class MappingComputer
{
public:
    static std::vector<Mapping> compute(
        const HighFive::Group &reportPop,
        const std::unordered_map<bbp::sonata::NodeID, Range> &nodePointers,
        const std::vector<bbp::sonata::NodeID> &nodeIds)
    {
        auto result = std::vector<Mapping>();
        auto elementIdsData = reportPop.getGroup("mapping").getDataSet("element_ids");
        for (auto nodeId : nodeIds)
        {
            auto it = nodePointers.find(nodeId);
            if (it == nodePointers.end())
            {
                continue;
            }

            auto elementIds = std::vector<bbp::sonata::ElementID>(it->second.second - it->second.first);
            elementIdsData.select({it->second.first}, {it->second.second - it->second.first}).read(elementIds.data());

            for (auto elem : elementIds)
            {
                result.push_back(std::make_pair(nodeId, elem));
            }
        }
        return result;
    }
};
} // namespace

namespace sonataloader
{
std::vector<bbp::sonata::NodeID> SimulationMapping::getCompartmentNodes(
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
std::vector<Mapping> SimulationMapping::getCompartmentMapping(
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

    if (!nodeIds.empty())
    {
        return MappingComputer::compute(reportPop, nodePointers, nodeIds);
    }

    return MappingComputer::compute(reportPop, nodePointers, reportNodeIds);
}
} // namespace sonataloader
