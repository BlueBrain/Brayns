/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#pragma once

#include <brayns/json/JsonAdapterMacro.h>
#include <brayns/json/JsonObjectMacro.h>

#include <io/NeuronMorphologyLoaderParameters.h>
#include <io/sonataloader/reports/ReportType.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_ENUM(
    sonataloader::ReportType,
    {"none", sonataloader::ReportType::None},
    {"spikes", sonataloader::ReportType::Spikes},
    {"compartment", sonataloader::ReportType::Compartment},
    {"summation", sonataloader::ReportType::Summation},
    {"synapse", sonataloader::ReportType::Synapse},
    {"bloodflow_pressure", sonataloader::ReportType::BloodflowPressure},
    {"bloodflow_speed", sonataloader::ReportType::BloodflowSpeed},
    {"bloodflow_radii", sonataloader::ReportType::BloodflowRadii})
}

// ---------------------------------------------------------------------------

BRAYNS_JSON_OBJECT_BEGIN(VasculatureGeometrySettings)
BRAYNS_JSON_OBJECT_ENTRY(
    float,
    radius_multiplier,
    "Factor by which to multiply all vasculature sample radii",
    brayns::Default(1.f),
    brayns::Minimum(0.1f))
BRAYNS_JSON_OBJECT_END()

BRAYNS_JSON_OBJECT_BEGIN(SonataEdgePopulationParameters)
BRAYNS_JSON_OBJECT_ENTRY(std::string, edge_population, "Name of the edge population to load")
BRAYNS_JSON_OBJECT_ENTRY(bool, load_afferent, "Wether to load afferent or efferent edges")
BRAYNS_JSON_OBJECT_ENTRY(
    float,
    edge_percentage,
    "Percentage of edges to load from all available",
    brayns::Minimum(0.001f),
    brayns::Maximum(1.f),
    brayns::Default(1.f))
BRAYNS_JSON_OBJECT_ENTRY(
    float,
    radius,
    "Radius used for the synapse sphere geometry (Ignored for endfeet)",
    brayns::Default(2.f),
    brayns::Minimum(0.1f))
BRAYNS_JSON_OBJECT_ENTRY(
    std::string,
    edge_report_name,
    "Name of a synapse report to load along the edge population",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_END()

// ---------------------------------------------------------------------------

BRAYNS_JSON_OBJECT_BEGIN(SonataNodePopulationParameters)
BRAYNS_JSON_OBJECT_ENTRY(std::string, node_population, "Name of the node population to load")
BRAYNS_JSON_OBJECT_ENTRY(
    std::vector<std::string>,
    node_sets,
    "List of node set names/regex to filter the node population load. Ignored if a list of node ids is provided",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    float,
    node_percentage,
    "Percentage of nodes to load after filter them by whichever node sets have "
    "been specified. Ignored if a lsit of node ids is provided",
    brayns::Default(0.01f),
    brayns::Minimum(0.001f),
    brayns::Maximum(1.f))
BRAYNS_JSON_OBJECT_ENTRY(
    std::vector<uint64_t>,
    node_ids,
    "List of node IDs to load from the population. Invalidates 'node_percentage' and 'node_sets'",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    sonataloader::ReportType,
    report_type,
    "Type of report to load for the given node population.",
    brayns::Default("none"))
BRAYNS_JSON_OBJECT_ENTRY(
    std::string,
    report_name,
    "Name of the report file to load (Ignored if report_type is 'none' or 'spikes')",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    float,
    spike_transition_time,
    "When loading a spike report, fade-in and fade-out time, in milliseconds, from "
    "resting state to spike state.",
    brayns::Default(1.f),
    brayns::Minimum(0.f))
BRAYNS_JSON_OBJECT_ENTRY(
    std::vector<SonataEdgePopulationParameters>,
    edge_populations,
    "List of edge populations and their settings to be loaded",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    NeuronMorphologyLoaderParameters,
    neuron_morphology_parameters,
    "Settings to configure the morphology geometry load. Ignored for vasculature populations",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    VasculatureGeometrySettings,
    vasculature_geometry_parameters,
    "Settings to configure the vasculature geometry load. Ignored for any node population that is not vasculature",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_END()

// ---------------------------------------------------------------------------

BRAYNS_JSON_OBJECT_BEGIN(SonataLoaderParameters)
BRAYNS_JSON_OBJECT_ENTRY(
    std::string,
    simulation_config_path,
    "Path to the simulation config file .json "
    "(By default will be searched in the same directory as the circuit config with name simulation_config.json)",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    std::vector<SonataNodePopulationParameters>,
    node_population_settings,
    "List of node populations to load and their load settings")
BRAYNS_JSON_OBJECT_END()
