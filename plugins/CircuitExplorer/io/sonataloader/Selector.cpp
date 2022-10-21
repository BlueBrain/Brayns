/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <io/sonataloader/data/SonataSimulationMapping.h>

#include <bbp/sonata/node_sets.h>

namespace
{
struct PercentageFilter
{
    static bbp::sonata::Selection filter(const bbp::sonata::Selection &selection, const float percentage) noexcept
    {
        const auto src = selection.flatten();

        const auto size = static_cast<float>(src.size());
        const auto expectedSize = size * percentage;
        auto skipFactor = static_cast<size_t>(size / expectedSize);
        skipFactor = std::max(skipFactor, 1ul);

        std::vector<uint64_t> finalList;
        finalList.reserve(expectedSize);

        for (size_t i = 0; i < src.size(); i = i + skipFactor)
        {
            finalList.push_back(src[i]);
        }

        return bbp::sonata::Selection::fromValues(finalList);
    }
};

struct NodeSetFilter
{
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
                result = result & newSelection;
            }

            return result;
        }

        return nodePopulation.selectAll();
    }
};

struct NodeReportFilter
{
    static bbp::sonata::Selection
        filter(const sonataloader::Config &config, const std::string &reportName, const std::string &population)
    {
        auto reportPath = config.getReportPath(reportName);
        auto nodeIds = sonataloader::SonataSimulationMapping::getCompartmentNodes(reportPath, population);
        std::sort(nodeIds.begin(), nodeIds.end());
        return bbp::sonata::Selection::fromValues(nodeIds);
    }
};
}

namespace sonataloader
{
bbp::sonata::Selection NodeSelector::select(const Config &config, const SonataNodePopulationParameters &params)
{
    const auto &nodePopulation = params.node_population;
    const auto &nodeSets = params.node_sets;
    const auto &nodeIds = params.node_ids;
    const auto reportType = params.report_type;
    const auto &reportName = params.report_name;
    const auto percentage = params.node_percentage;

    bbp::sonata::Selection reportSelection({});
    if (reportType != ReportType::None && reportType != ReportType::Spikes)
    {
        reportSelection = NodeReportFilter::filter(config, reportName, nodePopulation);
    }

    bbp::sonata::Selection result({});
    if (!nodeIds.empty())
    {
        result = bbp::sonata::Selection::fromValues(nodeIds);

        if (!reportSelection.empty())
        {
            result = result & reportSelection;
        }
    }
    else
    {
        result = NodeSetFilter::filter(config, nodePopulation, nodeSets);

        if (!reportSelection.empty())
        {
            result = result & reportSelection;
        }

        if (percentage < 1.f)
        {
            result = PercentageFilter::filter(result, percentage);
        }
    }

    if (result.empty())
    {
        throw std::runtime_error("Empty node selection for " + nodePopulation);
    }

    return result;
}

bbp::sonata::Selection EdgeSelector::select(
    const Config &config,
    const SonataEdgePopulationParameters &params,
    const bbp::sonata::Selection &baseNodes)
{
    auto &populationName = params.edge_population;
    auto population = config.getEdges(populationName);

    auto flatNodes = baseNodes.flatten();
    auto edgeSelection = population.afferentEdges(flatNodes);

    auto percentage = params.edge_percentage;
    return PercentageFilter::filter(edgeSelection, percentage);
}
}
