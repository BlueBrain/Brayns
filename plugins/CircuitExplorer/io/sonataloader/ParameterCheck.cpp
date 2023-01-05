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

#include <io/sonataloader/data/Names.h>
#include <io/sonataloader/data/PopulationType.h>

#include <filesystem>
#include <unordered_set>

namespace
{
namespace sl = sonataloader;

class NodeChecker
{
public:
    explicit NodeChecker(const sonataloader::Config &config)
        : _config(config)
    {
    }

    void check(const SonataNodePopulationParameters &params)
    {
        _checkPopulation(params);
        _checkNodeSets(params);
        _checkReport(params);
    }

private:
    void _checkPopulation(const SonataNodePopulationParameters &params)
    {
        auto nodeNames = _config.getAllNodeNames();
        if (nodeNames.find(params.node_population) == nodeNames.end())
        {
            throw std::invalid_argument("Population " + params.node_population + " does not exists");
        }
    }
    void _checkNodeSets(const SonataNodePopulationParameters &params)
    {
        auto &nodeSets = params.node_sets;
        if (nodeSets.empty())
        {
            return;
        }

        auto &nsPath = _config.getNodesetsPath();
        if (!std::filesystem::is_regular_file(nsPath))
        {
            throw std::invalid_argument("Cannot access nodesets file at " + nsPath);
        }
    }

    void _checkReport(const SonataNodePopulationParameters &params)
    {
        if (params.report_type == sl::ReportType::None)
        {
            return;
        }

        auto &population = params.node_population;
        auto populationType = sl::PopulationType::getNodeType(params.node_population, _config);

        switch (params.report_type)
        {
        case sl::ReportType::BloodflowPressure:
        case sl::ReportType::BloodflowRadii:
        case sl::ReportType::BloodflowSpeed:
        {
            if (populationType != sonataloader::NodeNames::vasculature)
            {
                throw std::invalid_argument("Invalid report type for node population " + population);
            }
            break;
        }
        case sl::ReportType::Compartment:
        case sl::ReportType::Summation:
        {
            if (populationType == sonataloader::NodeNames::vasculature)
            {
                throw std::invalid_argument("Invalid report type for node population " + population);
            }
            break;
        }
        case sl::ReportType::Spikes:
        {
            if (populationType == sonataloader::NodeNames::vasculature)
            {
                throw std::invalid_argument("Invalid report type for node population " + population);
            }
            break;
        }
        case sl::ReportType::Synapse:
        {
            throw std::invalid_argument("Invalid report type for node population " + population);
        }
        default:
            break;
        }

        auto &reportName = params.report_name;
        auto reportPath = _config.getReportPath(reportName);
        if (!std::filesystem::is_regular_file(reportPath))
        {
            throw std::invalid_argument("Cannot access report file for node population " + population);
        }
    }

    const sonataloader::Config &_config;
};

class EdgeChecker
{
public:
    explicit EdgeChecker(const sonataloader::Config &config)
        : _config(config)
    {
    }

    void check(const SonataNodePopulationParameters &params)
    {
        for (auto &edgeParams : params.edge_populations)
        {
            _checkPopulation(params.node_population, edgeParams);
            _checkSynapseAstrocyte(edgeParams);
            _checkReport(edgeParams);
        }
    }

private:
    void _checkPopulation(const std::string &nodePopulation, const SonataEdgePopulationParameters &params)
    {
        auto loadAfferent = params.load_afferent;
        auto edgePopulation = _config.getEdges(params.edge_population);

        if (loadAfferent && edgePopulation.target() != nodePopulation)
        {
            throw std::invalid_argument("Cannot load " + params.edge_population + " afferent for " + nodePopulation);
        }

        if (!loadAfferent && edgePopulation.source() != nodePopulation)
        {
            throw std::invalid_argument("Cannot load " + params.edge_population + " efferent for " + nodePopulation);
        }
    }

    void _checkSynapseAstrocyte(const SonataEdgePopulationParameters &params)
    {
        auto edgeProperties = _config.getEdgesProperties(params.edge_population);
        if (edgeProperties.type == sonataloader::EdgeNames::synapseAstrocyte && params.load_afferent)
        {
            throw std::invalid_argument("synapse_astrocyte edge populations are not allowed in afferent mode");
        }
    }

    void _checkReport(const SonataEdgePopulationParameters &params)
    {
        auto &reportName = params.edge_report_name;
        if (reportName.empty())
        {
            return;
        }
        auto reportPath = _config.getReportPath(reportName);
        if (!std::filesystem::is_regular_file(reportPath))
        {
            throw std::invalid_argument("Cannot access report file for edge population " + params.edge_population);
        }
    }

    const sonataloader::Config &_config;
};

class MorphologyChecker
{
public:
    explicit MorphologyChecker(const sl::Config &config)
        : _config(config)
    {
    }

    void check(const SonataNodePopulationParameters &params)
    {
        auto populationType = sl::PopulationType::getNodeType(params.node_population, _config);
        if (populationType != sl::NodeNames::biophysical && populationType != sl::NodeNames::astrocyte)
        {
            return;
        }

        auto &neuronParameters = params.neuron_morphology_parameters;
        auto soma = neuronParameters.load_soma;
        auto axon = neuronParameters.load_axon;
        auto dend = neuronParameters.load_dendrites;
        if (!soma && !axon && !dend)
        {
            throw std::invalid_argument("No morphology sections enabled for " + params.node_population);
        }
    }

private:
    const sl::Config &_config;
};
} // namespace

namespace sonataloader
{
void ParameterCheck::checkInput(const Config &config, const SonataLoaderParameters &input)
{
    auto &populations = input.node_population_settings;
    if (populations.empty())
    {
        throw std::invalid_argument("Input load parameters are empty");
    }

    auto nodeChecker = NodeChecker(config);
    auto edgeChecker = EdgeChecker(config);
    auto morphologyChecker = MorphologyChecker(config);

    for (auto &population : populations)
    {
        nodeChecker.check(population);
        edgeChecker.check(population);
        morphologyChecker.check(population);
    }
}
} // namespace sonataloader
