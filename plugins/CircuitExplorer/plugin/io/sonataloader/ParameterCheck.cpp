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

#include "ParameterCheck.h"

#include <bbp/sonata/node_sets.h>

#include <brayns/utils/Filesystem.h>

#include <plugin/io/sonataloader/data/SonataCells.h>

#include <unordered_set>

namespace sonataloader
{
namespace
{
void checkNodePopulation(const bbp::sonata::CircuitConfig& config,
                         const SonataNodePopulationParameters& population,
                         const std::string& populationType)
{
    const auto percentage = population.node_percentage;
    if (percentage <= 0.f || percentage > 1.f)
        throw std::invalid_argument(
            "SonataLoader: node_percentage must be > 0.0 and <= 1.0");

    const auto& nodeSets = population.node_sets;
    if (!nodeSets.empty())
    {
        const auto& nsPath = config.getNodeSetsPath();
        if (!fs::exists(nsPath))
            throw std::invalid_argument(
                "SonataLoader: Cannot find nodesets file (path='" + nsPath +
                "')");
        // Cant check nodesets, since regex are allowed and correctness
        // cannot be checked until materialization happens
    }

    const auto reportType = population.report_type;
    const auto reportTypeStr = brayns::enumToString<ReportType>(reportType);
    const auto& reportPath = population.report_path;
    if (reportType != ReportType::NONE)
    {
        if (!fs::exists(reportPath))
            throw std::invalid_argument(
                "SonataLoader: Cannot find report file for report '" +
                reportTypeStr + "'");
        switch (reportType)
        {
        case ReportType::BLOODFLOW_PRESSURE:
        case ReportType::BLOODFLOW_RADII:
        case ReportType::BLOODFLOW_SPEED:
        {
            if (populationType != "vasculature")
                throw std::invalid_argument(
                    "SonataLoader: Report type '" + reportTypeStr +
                    "' is allowed only for vasculature populations");
            break;
        }
        case ReportType::COMPARTMENT:
        case ReportType::SPIKES:
        case ReportType::SUMMATION:
        {
            if (populationType == "vasculature")
                throw std::invalid_argument(
                    "SonataLoader: Report type '" + reportTypeStr +
                    "' is not allowed for vasculature populations");
            break;
        }
        case ReportType::SYNAPSE:
            throw std::invalid_argument(
                "SonataLoader: 'synapse' reports are not allowed on node "
                "populations");
        default:
            break;
        }
    }
}

void checkEdges(const bbp::sonata::CircuitConfig& config,
                const SonataNodePopulationParameters& population,
                const std::string& populationType)
{
    const auto allEdges = config.listEdgePopulations();
    for (const auto& edge : population.edge_populations)
    {
        const auto& name = edge.edge_population;
        if (allEdges.find(name) == allEdges.end())
            throw std::invalid_argument("SonataLoader: Edge population " +
                                        name + " does not exist");

        if (edge.edge_percentage <= 0.f || edge.edge_percentage > 1.f)
            throw std::invalid_argument(
                "SonataLoader: edge_percentage must be > 0.0 and <= 1.0");

        const auto edgePopulation = config.getEdgePopulation(name);
        if (edge.load_afferent &&
            edgePopulation.target() != population.node_population)
            throw std::invalid_argument(
                "SonataLoader: Edge population " + name +
                " requested in afferent mode but node population " +
                population.node_population + " is not its target");
        else if (!edge.load_afferent &&
                 edgePopulation.source() != population.node_population)
            throw std::invalid_argument(
                "SonataLoader: Edge population " + name +
                " requested in efferent mode but node population " +
                population.node_population + " is not its source");

        const auto edgeProperties = config.getEdgePopulationProperties(name);
        if (edgeProperties.type == "synapse_astrocyte" &&
            populationType == "biophysical")
            throw std::invalid_argument(
                "SonataLoader: synapse_astrocyte edge populations are not "
                "allowed for biophysical node populations");

        if (!edge.edge_report.empty() && !fs::exists(edge.edge_report))
            throw std::invalid_argument(
                "SonataLoader: Requested report for edge population " + name +
                " not found");
    }
}

void checkNeuronMorphology(const SonataNodePopulationParameters& population,
                           const std::string& populationType)
{
    if (populationType == "vasculature")
        return;

    const auto& neuronParameters = population.neuron_morphology_parameters;
    if (populationType == "biophysical" || populationType == "astrocyte")
    {
        if (!neuronParameters.load_soma && !neuronParameters.load_axon &&
            !neuronParameters.load_dendrites)
            throw std::invalid_argument(
                "SonataLoader: At least one neuron section must be requested "
                "to load 'biophysial' or 'astrocyte' population types");
    }

    if (neuronParameters.radius_override < 0.f)
        throw std::invalid_argument(
            "SonataLoader: 'radius_override' must be >= 0.0");
    else if (neuronParameters.radius_override == 0.f &&
             neuronParameters.radius_multiplier <= 0.f)
        throw std::invalid_argument(
            "SonataLoader: 'radius_multiplier' must be > 0.0");
}

void checkVasculature(const SonataNodePopulationParameters& population,
                      const std::string& populationType)
{
    if (populationType != "vasculature")
        return;

    const auto& vascParameters = population.vasculature_geometry_parameters;

    if (vascParameters.radius_override < 0.f)
        throw std::invalid_argument(
            "SonataLoader: 'radius_override' must be >= 0.0");
    else if (vascParameters.radius_override == 0.f &&
             vascParameters.radius_multiplier <= 0.f)
        throw std::invalid_argument(
            "SonataLoader: 'radius_multiplier' must be > 0.0");
}
} // namespace

void ParameterCheck::checkInput(const bbp::sonata::CircuitConfig& config,
                                const SonataLoaderParameters& input)
{
    if (input.node_population_settings.empty())
        throw std::invalid_argument("Input load parameters are empty");

    for (const auto& population : input.node_population_settings)
    {
        const auto& name = population.node_population;
        const auto allNodes = config.listNodePopulations();
        if (allNodes.find(name) == allNodes.end())
            throw std::invalid_argument("SonataLoader: Node population " +
                                        name + " does not exist");

        const auto type = config.getNodePopulationProperties(name).type;

        checkNodePopulation(config, population, type);
        checkEdges(config, population, type);
        checkNeuronMorphology(population, type);
        checkVasculature(population, type);
    }
}
} // namespace sonataloader
