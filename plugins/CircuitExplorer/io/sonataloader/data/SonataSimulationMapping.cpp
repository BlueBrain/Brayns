
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
