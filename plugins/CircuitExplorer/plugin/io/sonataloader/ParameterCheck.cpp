/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include <unordered_set>

namespace sonataloader
{
namespace
{
void __checkNodePopulation(const bbp::sonata::CircuitConfig& config,
                           const SonataNodePopulationParameters& population,
                           const bbp::sonata::PopulationProperties& nodeProperties)
{
    const auto percentage = population.node_percentage;
    if(percentage <= 0.f || percentage > 1.f)
        throw std::invalid_argument("SonataLoader: node_percentage must be > 0.0 and <= 1.0");

    const auto& nodeSets = population.node_sets;
    if(!nodeSets.empty())
    {
        const auto& nsPath = config.getNodeSetsPath();
        if(!fs::exists(nsPath))
            throw std::invalid_argument("SonataLoader: Cannot find nodesets file (path='"+nsPath+"')");
        // Cant check nodesets, since regex are allowed and correctness
        // cannot be checked until materialization happens
    }

    const auto reportType = population.report_type;
    const auto reportTypeStr = brayns::enumToString<ReportType>(reportType);
    const auto& reportPath = population.report_path;
    if(reportType != ReportType::NONE)
    {
        if(!fs::exists(reportPath))
            throw std::invalid_argument("SonataLoader: Cannot find report file for report '"+reportTypeStr+"'");
        switch(reportType)
        {
        case ReportType::BLOODFLOW_PRESSURE:
        case ReportType::BLOODFLOW_RADII:
        case ReportType::BLOODFLOW_SPEED:
        {
            if(nodeProperties.type != "vasculature")
                throw std::invalid_argument("SonataLoader: Report type '"+reportTypeStr+"' is allowed only for vasculature populations");
            break;
        }
        case ReportType::COMPARTMENT:
        case ReportType::SPIKES:
        case ReportType::SUMMATION:
        {
            if(nodeProperties.type == "vasculature")
                throw std::invalid_argument("SonataLoader: Report type '"+reportTypeStr+"' is not allowed for vasculature populations");
            break;
        }
        case ReportType::SYNAPSE:
            throw std::invalid_argument("SonataLoader: 'synapse' reports are not allowed on node populations");
        default:
            break;
        }
    }
}

void __checkEdges(const bbp::sonata::CircuitConfig& config,
                  const SonataNodePopulationParameters& population,
                  const bbp::sonata::PopulationProperties& nodeProperties)
{
    const auto allEdges = config.listEdgePopulations();
    for(const auto& edge : population.edge_populations)
    {
        const auto& name = edge.edge_population;
        if(allEdges.find(name) == allEdges.end())
            throw std::invalid_argument("SonataLoader: Edge population "+name+" does not exist");

        if(edge.edge_percentage <= 0.f || edge.edge_percentage > 1.f)
            throw std::invalid_argument("SonataLoader: edge_percentage must be > 0.0 and <= 1.0");

        const auto edgePopulation = config.getEdgePopulation(name);
        if(edge.load_afferent && edgePopulation.target() != population.node_population)
            throw std::invalid_argument("SonataLoader: Edge population "+name+" requested in afferent mode but node population "+population.node_population+" is not its target");
        else if(!edge.load_afferent && edgePopulation.source() != population.node_population)
            throw std::invalid_argument("SonataLoader: Edge population "+name+" requested in efferent mode but node population "+population.node_population+" is not its source");

        const auto edgeProperties = config.getEdgePopulationProperties(name);
        if(edgeProperties.type == "synapse_astrocyte" && nodeProperties.type == "biophysical")
            throw std::invalid_argument("SonataLoader: synapse_astrocyte edge populations are not allowed for biophysical node populations");

        if(!edge.edge_report.empty() && !fs::exists(edge.edge_report))
            throw std::invalid_argument("SonataLoader: Requested report for edge population "+name+" not found");
    }
}

void __checkNeuronMorphology(const SonataNodePopulationParameters& population,
                             const bbp::sonata::PopulationProperties& nodeProperties)
{
    const auto& neuronParameters = population.neuron_morphology_parameters;
    if(nodeProperties.type == "biophysical" || nodeProperties.type == "astrocyte")
    {
        if(!neuronParameters.load_soma
                && !neuronParameters.load_axon
                && !neuronParameters.load_dendrites)
            throw std::invalid_argument("SonataLoader: At least one neuron section must be requested to load 'biophysial' or 'astrocyte' population types");
    }

    if(neuronParameters.radius_override < 0.f)
        throw std::invalid_argument("SonataLoader: 'radius_override' must be >= 0.0");
    else if(neuronParameters.radius_override == 0.f
            && neuronParameters.radius_multiplier <= 0.f)
        throw std::invalid_argument("SonataLoader: 'radius_multiplier' must be > 0.0");
}

void __checkVasculature(const SonataNodePopulationParameters& population,
                        const bbp::sonata::PopulationProperties& nodeProperties)
{
    const auto& vascParameters = population.vasculature_geometry_parameters;
    if(nodeProperties.type == "vasculature")
    {
        if(!vascParameters.load_arterial_capillary
                && !vascParameters.load_arteriole
                && !vascParameters.load_artery
                && !vascParameters.load_transitional
                && !vascParameters.load_vein
                && !vascParameters.load_venous_capillary
                && !vascParameters.load_venule)
            throw std::invalid_argument("SonataLoader: At least one vasculature section must be requested to load 'vasculature' population type");
    }

    if(vascParameters.radius_override < 0.f)
        throw std::invalid_argument("SonataLoader: 'radius_override' must be >= 0.0");
    else if(vascParameters.radius_override == 0.f
            && vascParameters.radius_multiplier <= 0.f)
        throw std::invalid_argument("SonataLoader: 'radius_multiplier' must be > 0.0");
}
} // namespace

void ParameterCheck::checkInput(const bbp::sonata::CircuitConfig& config, const SonataLoaderParameters& input)
{
    if(input.node_population_settings.empty())
        throw std::invalid_argument("Input load parameters are empty");

    for(const auto& population : input.node_population_settings)
    {
        const auto& name = population.node_population;
        const auto allNodes = config.listNodePopulations();
        if(allNodes.find(name) == allNodes.end())
            throw std::invalid_argument("SonataLoader: Node population "+name+" does not exist");

        const auto nodeProperties = config.getNodePopulationProperties(name);

        __checkNodePopulation(config, population, nodeProperties);
        __checkEdges(config, population, nodeProperties);
        __checkNeuronMorphology(population, nodeProperties);
        __checkVasculature(population, nodeProperties);
    }
}

} // namespace sonataloader
