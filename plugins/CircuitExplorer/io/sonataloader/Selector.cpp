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

#include <io/sonataloader/data/SonataConfig.h>
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
        const auto skipFactor = static_cast<size_t>(size / expectedSize);

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
        const bbp::sonata::CircuitConfig &config,
        const std::string &population,
        const std::vector<std::string> &nodeSets)
    {
        const auto nodePopulation = config.getNodePopulation(population);

        if (!nodeSets.empty())
        {
            bbp::sonata::Selection result({});
            const auto &nodeSetsPath = config.getNodeSetsPath();
            const auto nodeSetFile = bbp::sonata::NodeSets::fromFile(nodeSetsPath);
            for (const auto &nodeSetName : nodeSets)
            {
                const auto newSelection = nodeSetFile.materialize(nodeSetName, nodePopulation);
                result = result & newSelection;
            }

            return result;
        }

        return nodePopulation.selectAll();
    }
};

struct NodeReportFilter
{
    static bbp::sonata::Selection filter(
        const bbp::sonata::SimulationConfig &config,
        const std::string &reportName,
        const std::string &population)
    {
        auto reportPath = sonataloader::SonataConfig::resolveReportPath(config, reportName);

        auto nodeIds = sonataloader::SonataSimulationMapping::getCompartmentNodes(reportPath, population);
        std::sort(nodeIds.begin(), nodeIds.end());
        return bbp::sonata::Selection::fromValues(nodeIds);
    }
};
}

namespace sonataloader
{
bbp::sonata::Selection NodeSelector::select(
    const SonataNetworkConfig &network,
    const SonataNodePopulationParameters &params)
{
    const auto &nodePopulation = params.node_population;
    const auto &nodeSets = params.node_sets;
    const auto &nodeIds = params.node_ids;
    const auto reportType = params.report_type;
    const auto &reportName = params.report_name;
    const auto percentage = params.node_percentage;
    const auto &config = network.circuitConfig();

    bbp::sonata::Selection reportSelection({});
    if (reportType != ReportType::None && reportType != ReportType::Spikes)
    {
        const auto &simConfig = network.simulationConfig();
        reportSelection = NodeReportFilter::filter(simConfig, reportName, nodePopulation);
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
    const SonataNetworkConfig &network,
    const SonataEdgePopulationParameters &params,
    const bbp::sonata::Selection &baseNodes)
{
    const auto &config = network.circuitConfig();
    const auto &populationName = params.edge_population;
    const auto population = config.getEdgePopulation(populationName);

    const auto flatNodes = baseNodes.flatten();
    const auto edgeSelection = population.afferentEdges(flatNodes);

    const auto percentage = params.edge_percentage;
    return PercentageFilter::filter(edgeSelection, percentage);
}
}
