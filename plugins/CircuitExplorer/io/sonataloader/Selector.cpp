/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "Selector.h"

#include <io/sonataloader/data/SimulationMapping.h>

#include <bbp/sonata/node_sets.h>

namespace
{
class NodeSetFilter
{
public:
    static bbp::sonata::Selection filter(
        const sonataloader::Config &config,
        const std::string &population,
        const std::vector<std::string> &nodeSets)
    {
        auto nodePopulation = config.getNodes(population);
        if (!nodeSets.empty())
        {
            bbp::sonata::Selection result({});
            auto &nodeSetsPath = config.getNodesetsPath();
            auto nodeSetFile = bbp::sonata::NodeSets::fromFile(nodeSetsPath);
            for (auto &nodeSetName : nodeSets)
            {
                auto newSelection = nodeSetFile.materialize(nodeSetName, nodePopulation);
                result = result | newSelection;
            }

            return result;
        }

        return nodePopulation.selectAll();
    }
};

class NodeReportFilter
{
public:
    static bbp::sonata::Selection filter(
        const sonataloader::Config &config,
        const std::string &reportName,
        const std::string &population)
    {
        auto reportPath = config.getReportPath(reportName);
        auto nodeIds = sonataloader::SimulationMapping::getCompartmentNodes(reportPath, population);
        std::sort(nodeIds.begin(), nodeIds.end());
        return bbp::sonata::Selection::fromValues(nodeIds);
    }
};

class SelectionChecker
{
public:
    static void check(const bbp::sonata::Selection &selection, const std::string &population)
    {
        if (selection.empty())
        {
            throw std::runtime_error("Empty node selection for " + population);
        }
    }
};

class NodeCountLimiter
{
public:
    static bbp::sonata::Selection limit(const bbp::sonata::Selection &selection, size_t limit)
    {
        if (limit == 0 || selection.flatSize() <= limit)
        {
            return selection;
        }

        auto percentage = static_cast<float>(limit) / static_cast<float>(selection.flatSize());
        return sonataloader::PercentageFilter::filter(selection, percentage);
    }
};
}

namespace sonataloader
{
bbp::sonata::Selection PercentageFilter::filter(const bbp::sonata::Selection &selection, float percentage) noexcept
{
    if (percentage >= 1.f)
    {
        return selection;
    }

    auto src = selection.flatten();
    auto size = static_cast<float>(src.size());
    auto expectedSize = size * percentage;
    auto skipFactor = static_cast<size_t>(size / expectedSize);
    skipFactor = std::max(skipFactor, 1ul);

    std::vector<uint64_t> finalList;
    finalList.reserve(static_cast<size_t>(expectedSize));

    for (size_t i = 0; i < src.size(); i = i + skipFactor)
    {
        finalList.push_back(src[i]);
    }

    return bbp::sonata::Selection::fromValues(finalList);
}

bbp::sonata::Selection NodeSelector::select(const Config &config, const SonataNodePopulationParameters &params)
{
    auto &nodePopulation = params.node_population;
    auto &nodeSets = params.node_sets;
    auto &nodeIds = params.node_ids;
    auto reportType = params.report_type;
    auto percentage = params.node_percentage;

    bbp::sonata::Selection reportSelection({});
    if (reportType != ReportType::None && reportType != ReportType::Spikes)
    {
        auto &reportName = params.report_name;
        reportSelection = NodeReportFilter::filter(config, reportName, nodePopulation);
    }

    if (!nodeIds.empty())
    {
        auto result = bbp::sonata::Selection::fromValues(nodeIds);

        if (!reportSelection.empty())
        {
            result = result & reportSelection;
        }
        SelectionChecker::check(result, nodePopulation);
        return NodeCountLimiter::limit(result, params.node_count_limit);
    }

    auto result = NodeSetFilter::filter(config, nodePopulation, nodeSets);

    if (!reportSelection.empty())
    {
        result = result & reportSelection;
    }

    result = PercentageFilter::filter(result, percentage);

    SelectionChecker::check(result, nodePopulation);
    return NodeCountLimiter::limit(result, params.node_count_limit);
}

bbp::sonata::Selection EdgeSelector::select(
    const Config &config,
    const SonataEdgePopulationParameters &params,
    const bbp::sonata::Selection &baseNodes)
{
    auto &populationName = params.edge_population;
    auto population = config.getEdges(populationName);

    auto flatNodes = baseNodes.flatten();
    auto edgeSelection =
        params.load_afferent ? population.afferentEdges(flatNodes) : population.efferentEdges(flatNodes);

    auto percentage = params.edge_percentage;
    return PercentageFilter::filter(edgeSelection, percentage);
}
}
