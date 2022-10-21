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
#include <io/sonataloader/data/SonataNames.h>

#include <filesystem>
#include <unordered_set>

namespace
{
namespace sl = sonataloader;

struct PopulationTypeExtractor
{
    static std::string extract(const sl::Config &config, const SonataNodePopulationParameters &params)
    {
        auto &populationName = params.node_population;
        auto populationProperties = config.getNodesProperties(populationName);
        return populationProperties.type;
    }
};

void checkNodePopulation(const sl::Config &config, const SonataNodePopulationParameters &params)
{
    auto populationType = PopulationTypeExtractor::extract(config, params);
    auto &nodeSets = params.node_sets;
    auto &population = params.node_population;

    if (!nodeSets.empty())
    {
        auto &nsPath = config.getNodesetsPath();
        if (!std::filesystem::is_regular_file(nsPath))
        {
            throw std::invalid_argument("Cannot access nodesets file for node population " + population);
        }
    }

    if (params.report_type == sl::ReportType::None)
    {
        return;
    }

    switch (params.report_type)
    {
    case sl::ReportType::BloodflowPressure:
    case sl::ReportType::BloodflowRadii:
    case sl::ReportType::BloodflowSpeed:
    {
        if (populationType != sonataloader::SonataNodeNames::vasculature)
        {
            throw std::invalid_argument("Invalid report type for node population " + population);
        }
        break;
    }
    case sl::ReportType::Compartment:
    case sl::ReportType::Summation:
    {
        if (populationType == sonataloader::SonataNodeNames::vasculature)
        {
            throw std::invalid_argument("Invalid report type for node population " + population);
        }
        break;
    }
    case sl::ReportType::Spikes:
    {
        if (populationType == sonataloader::SonataNodeNames::vasculature)
        {
            throw std::invalid_argument("Invalid report type for node population " + population);
        }
        break;
    }
    case sl::ReportType::Synapse:
    {
        throw std::invalid_argument("Invalid report type for node population " + population);
        break;
    }
    default:
        break;
    }

    auto &reportName = params.report_name;
    auto reportPath = config.getReportPath(reportName);
    if (!std::filesystem::is_regular_file(reportPath))
    {
        throw std::invalid_argument("Cannot access report file for node population " + population);
    }
}

void checkEdges(const sl::Config &config, const SonataNodePopulationParameters &params)
{
    auto allEdges = config.getAllEdgeNames();
    auto &nodePopulation = params.node_population;
    for (auto &edge : params.edge_populations)
    {
        auto &name = edge.edge_population;
        if (allEdges.find(name) == allEdges.end())
        {
            throw std::invalid_argument("Edge population " + name + " does not exist");
        }

        auto loadAfferent = edge.load_afferent;
        auto edgePopulation = config.getEdges(name);

        if (loadAfferent && edgePopulation.target() != nodePopulation)
        {
            throw std::invalid_argument(
                "Edge population " + name + " requested in afferent mode but node population " + nodePopulation
                + " is not its target");
        }

        if (!loadAfferent && edgePopulation.source() != nodePopulation)
        {
            throw std::invalid_argument(
                "Edge population " + name + " requested in efferent mode but node population " + nodePopulation
                + " is not its source");
        }

        auto edgeProperties = config.getEdgesProperties(name);
        if (edgeProperties.type == sonataloader::SonataEdgeNames::synapseAstrocyte && loadAfferent)
        {
            throw std::invalid_argument("synapse_astrocyte edge populations are not allowed in afferent mode");
        }

        auto &reportName = edge.edge_report_name;
        if (reportName.empty())
        {
            return;
        }
        auto reportPath = config.getReportPath(reportName);
        if (!std::filesystem::is_regular_file(reportPath))
        {
            throw std::invalid_argument("Cannot access report file for edge population " + name);
        }
    }
}

void checkNeuronMorphology(const sl::Config &config, const SonataNodePopulationParameters &params)
{
    auto populationType = PopulationTypeExtractor::extract(config, params);

    if (populationType == "vasculature")
    {
        return;
    }

    auto &neuronParameters = params.neuron_morphology_parameters;
    if (populationType == "biophysical" || populationType == "astrocyte")
    {
        auto soma = neuronParameters.load_soma;
        auto axon = neuronParameters.load_axon;
        auto dend = neuronParameters.load_dendrites;
        if (!soma && !axon && !dend)
        {
            auto &populationName = params.node_population;
            throw std::invalid_argument("Must enable at least one neuron morphology section for " + populationName);
        }
    }
}
} // namespace

namespace sonataloader
{
void ParameterCheck::checkInput(const Config &config, const SonataLoaderParameters &input)
{
    const auto &populations = input.node_population_settings;
    if (populations.empty())
    {
        throw std::invalid_argument("Input load parameters are empty");
    }

    for (auto &population : populations)
    {
        auto &name = population.node_population;
        auto allNodes = config.getAllNodeNames();
        if (allNodes.find(name) == allNodes.end())
        {
            throw std::invalid_argument("Node population " + name + " does not exist");
        }

        checkNodePopulation(config, population);
        checkEdges(config, population);
        checkNeuronMorphology(config, population);
    }
}
} // namespace sonataloader
