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

#include "ParameterCheck.h"

#include <bbp/sonata/node_sets.h>

#include <io/sonataloader/data/SonataCells.h>
#include <io/sonataloader/data/SonataConfig.h>

#include <filesystem>
#include <unordered_set>

namespace
{
namespace sl = sonataloader;

struct PopulationTypeExtractor
{
    static std::string extract(const sl::SonataNetworkConfig &network, const SonataNodePopulationParameters &params)
    {
        const auto &config = network.circuitConfig();
        const auto &populationName = params.node_population;
        const auto populationProperties = config.getNodePopulationProperties(populationName);
        return populationProperties.type;
    }
};

void checkNodePopulation(const sl::SonataNetworkConfig &network, const SonataNodePopulationParameters &params)
{
    const auto &config = network.circuitConfig();
    const auto populationType = PopulationTypeExtractor::extract(network, params);
    const auto &nodeSets = params.node_sets;
    const auto &population = params.node_population;

    if (!nodeSets.empty())
    {
        const auto &nsPath = config.getNodeSetsPath();
        if (!std::filesystem::exists(nsPath))
        {
            throw std::invalid_argument("Cannot find nodesets file for node population " + population);
        }
    }

    const auto reportType = params.report_type;
    const auto &reportName = params.report_name;

    if (reportType != sl::ReportType::NONE)
    {
        const auto &simConfig = network.simulationConfig();
        std::string reportPath;

        switch (reportType)
        {
        case sl::ReportType::BLOODFLOW_PRESSURE:
        case sl::ReportType::BLOODFLOW_RADII:
        case sl::ReportType::BLOODFLOW_SPEED:
        {
            if (populationType != "vasculature")
            {
                throw std::invalid_argument("Invalid report type for node population " + population);
            }
            reportPath = sl::SonataConfig::resolveReportPath(simConfig, reportName);
            break;
        }
        case sl::ReportType::COMPARTMENT:
        case sl::ReportType::SUMMATION:
        {
            if (populationType == "vasculature")
            {
                throw std::invalid_argument("Invalid report type for node population " + population);
            }
            reportPath = sl::SonataConfig::resolveReportPath(simConfig, reportName);
            break;
        }
        case sl::ReportType::SPIKES:
        {
            if (populationType == "vasculature")
            {
                throw std::invalid_argument("Invalid report type for node population " + population);
            }
            reportPath = sl::SonataConfig::resolveSpikesPath(simConfig);
            break;
        }
        case sl::ReportType::SYNAPSE:
        {
            throw std::invalid_argument("Invalid report type for node population " + population);
            break;
        }
        default:
            break;
        }

        if (!std::filesystem::exists(reportPath))
        {
            throw std::invalid_argument("Cannot find report file for node population " + population);
        }
    }
}

void checkEdges(const sl::SonataNetworkConfig &network, const SonataNodePopulationParameters &params)
{
    const auto &config = network.circuitConfig();
    const auto allEdges = config.listEdgePopulations();
    const auto &nodePopulation = params.node_population;
    for (const auto &edge : params.edge_populations)
    {
        const auto &name = edge.edge_population;
        if (allEdges.find(name) == allEdges.end())
        {
            throw std::invalid_argument("Edge population " + name + " does not exist");
        }

        const auto loadAfferent = edge.load_afferent;
        const auto edgePopulation = config.getEdgePopulation(name);

        if (loadAfferent && edgePopulation.target() != nodePopulation)
        {
            throw std::invalid_argument(
                "Edge population " + name + " requested in afferent mode but node population " + nodePopulation
                + " is not its target");
        }
        else if (!loadAfferent && edgePopulation.source() != nodePopulation)
        {
            throw std::invalid_argument(
                "Edge population " + name + " requested in efferent mode but node population " + nodePopulation
                + " is not its source");
        }

        const auto edgeProperties = config.getEdgePopulationProperties(name);
        if (edgeProperties.type == "synapse_astrocyte" && loadAfferent)
        {
            throw std::invalid_argument("synapse_astrocyte edge populations are not allowed in afferent mode");
        }

        const auto &reportName = edge.edge_report_name;
        if (!reportName.empty())
        {
            const auto &simConfig = network.simulationConfig();
            const auto reportPath = sl::SonataConfig::resolveReportPath(simConfig, reportName);
            if (!std::filesystem::exists(reportPath))
            {
                throw std::invalid_argument("Requested report for edge population " + name + " not found");
            }
        }
    }
}

void checkNeuronMorphology(const sl::SonataNetworkConfig &network, const SonataNodePopulationParameters &params)
{
    const auto populationType = PopulationTypeExtractor::extract(network, params);

    if (populationType == "vasculature")
    {
        return;
    }

    const auto &neuronParameters = params.neuron_morphology_parameters;
    if (populationType == "biophysical" || populationType == "astrocyte")
    {
        const auto soma = neuronParameters.load_soma;
        const auto axon = neuronParameters.load_axon;
        const auto dend = neuronParameters.load_dendrites;
        if (!soma && !axon && !dend)
        {
            const auto &populationName = params.node_population;
            throw std::invalid_argument("Must enable at least one neuron morphology section for " + populationName);
        }
    }
}
} // namespace

namespace sonataloader
{
void ParameterCheck::checkInput(const SonataNetworkConfig &network, const SonataLoaderParameters &input)
{
    const auto &populations = input.node_population_settings;
    if (populations.empty())
    {
        throw std::invalid_argument("Input load parameters are empty");
    }

    const auto &config = network.circuitConfig();

    for (const auto &population : populations)
    {
        const auto &name = population.node_population;
        const auto allNodes = config.listNodePopulations();
        if (allNodes.find(name) == allNodes.end())
        {
            throw std::invalid_argument("Node population " + name + " does not exist");
        }

        checkNodePopulation(network, population);
        checkEdges(network, population);
        checkNeuronMorphology(network, population);
    }
}
} // namespace sonataloader
