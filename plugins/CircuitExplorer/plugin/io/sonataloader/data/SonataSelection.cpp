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

#include "SonataSelection.h"

#include <bbp/sonata/node_sets.h>
#include <bbp/sonata/report_reader.h>

#include <plugin/io/sonataloader/data/SonataSimulationMapping.h>

namespace sonataloader
{
namespace
{
bbp::sonata::Selection applyPercentage(const std::vector<uint64_t>& src,
                                       const float percentage) noexcept
{
    const auto expectedSize = static_cast<size_t>(src.size() * percentage);
    const auto skipFactor = static_cast<size_t>(
        static_cast<float>(src.size()) / static_cast<float>(expectedSize));
    std::vector<uint64_t> finalList;
    finalList.reserve(expectedSize);
    for (size_t i = 0; i < src.size(); i = i + skipFactor)
        finalList.push_back(src[i]);

    return bbp::sonata::Selection::fromValues(finalList);
}
} // namespace

NodeSelection::NodeSelection()
    : _nodeSetsSelection({})
    , _nodeListSelection({})
    , _simulationSelection({})
{
}

void NodeSelection::select(const bbp::sonata::CircuitConfig& config,
                           const std::string& population,
                           const std::vector<std::string>& nodeSets)
{
    const auto nodePopulation = config.getNodePopulation(population);

    if (!nodeSets.empty())
    {
        const auto nodeSetFile =
            bbp::sonata::NodeSets::fromFile(config.getNodeSetsPath());
        for (const auto& nodeSetName : nodeSets)
        {
            const auto newSelection =
                nodeSetFile.materialize(nodeSetName, nodePopulation);
            _nodeSetsSelection = _nodeListSelection & newSelection;
        }
    }
    else
        _nodeSetsSelection = nodePopulation.selectAll();
}

void NodeSelection::select(const std::vector<uint64_t>& nodeList)
{
    _nodeListSelection = bbp::sonata::Selection::fromValues(nodeList);
}

void NodeSelection::select(const ReportType simType,
                           const std::string& reportPath,
                           const std::string& population)
{
    if (simType == ReportType::SPIKES || simType == ReportType::NONE)
        return;

    auto nodeIds =
        SonataSimulationMapping::getCompartmentNodes(reportPath, population);
    std::sort(nodeIds.begin(), nodeIds.end());
    _simulationSelection = bbp::sonata::Selection::fromValues(nodeIds);
}

bbp::sonata::Selection NodeSelection::intersection(const float percent)
{
    // Specified list of nodes have preference
    if (!_nodeListSelection.empty())
    {
        // If also report is specified, return only the intersection
        if (!_simulationSelection.empty())
            return _nodeListSelection & _simulationSelection;
        else
            return _nodeListSelection;
    }
    else
    {
        bbp::sonata::Selection common({});
        if (!_simulationSelection.empty())
            common = _nodeSetsSelection & _simulationSelection;
        else
            common = _nodeSetsSelection;

        if (percent < 1.0)
            common = applyPercentage(common.flatten(), percent);

        return common;
    }
}

EdgeSelection::EdgeSelection(bbp::sonata::Selection&& intialEdgeSelection)
    : _edgeListSelection(std::move(intialEdgeSelection))
{
}

bbp::sonata::Selection EdgeSelection::intersection(const float percentage)
{
    if (percentage < 1.f)
        return applyPercentage(_edgeListSelection.flatten(), percentage);

    return _edgeListSelection;
}
} // namespace sonataloader
