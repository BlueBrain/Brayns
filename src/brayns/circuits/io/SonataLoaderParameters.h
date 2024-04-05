/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/core/json/Json.h>

#include <brayns/circuits/io/NeuronMorphologyLoaderParameters.h>
#include <brayns/circuits/io/sonataloader/reports/ReportType.h>

struct VasculatureGeometrySettings
{
    float radius_multiplier = 0;
};

struct SonataEdgePopulationParameters
{
    std::string edge_population;
    bool load_afferent = false;
    float edge_percentage = 0;
    float radius = 0;
    std::string edge_report_name;
};

struct SonataNodePopulationParameters
{
    std::string node_population;
    std::vector<std::string> node_sets;
    float node_percentage = 0;
    std::vector<uint64_t> node_ids;
    size_t node_count_limit = 0;
    sonataloader::ReportType report_type = sonataloader::ReportType::None;
    std::string report_name;
    float spike_transition_time = 0;
    std::vector<SonataEdgePopulationParameters> edge_populations;
    NeuronMorphologyLoaderParameters neuron_morphology_parameters;
    VasculatureGeometrySettings vasculature_geometry_parameters;
};

struct SonataLoaderParameters
{
    std::vector<SonataNodePopulationParameters> node_population_settings;
};

namespace brayns
{
template<>
struct EnumReflector<sonataloader::ReportType>
{
    static EnumMap<sonataloader::ReportType> reflect()
    {
        return {
            {"none", sonataloader::ReportType::None},
            {"spikes", sonataloader::ReportType::Spikes},
            {"compartment", sonataloader::ReportType::Compartment},
            {"summation", sonataloader::ReportType::Summation},
            {"synapse", sonataloader::ReportType::Synapse},
            {"bloodflow_pressure", sonataloader::ReportType::BloodflowPressure},
            {"bloodflow_speed", sonataloader::ReportType::BloodflowSpeed},
            {"bloodflow_radii", sonataloader::ReportType::BloodflowRadii}};
    }
};

template<>
struct JsonAdapter<sonataloader::ReportType> : EnumAdapter<sonataloader::ReportType>
{
};

template<>
struct JsonAdapter<VasculatureGeometrySettings> : ObjectAdapter<VasculatureGeometrySettings>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("VasculatureGeometrySettings");
        builder
            .getset(
                "radius_multiplier",
                [](auto &object) { return object.radius_multiplier; },
                [](auto &object, auto value) { object.radius_multiplier = value; })
            .description("Factor to multiply all vasculature sample radii")
            .minimum(0.1)
            .defaultValue(1);
        return builder.build();
    }
};

template<>
struct JsonAdapter<SonataEdgePopulationParameters> : ObjectAdapter<SonataEdgePopulationParameters>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("SonataEdgePopulationParameters");
        builder
            .getset(
                "edge_population",
                [](auto &object) -> auto & { return object.edge_population; },
                [](auto &object, auto value) { object.edge_population = std::move(value); })
            .description("Name of the edge population to load");
        builder
            .getset(
                "load_afferent",
                [](auto &object) { return object.load_afferent; },
                [](auto &object, auto value) { object.load_afferent = value; })
            .description("Wether to load afferent or efferent edges");
        builder
            .getset(
                "edge_percentage",
                [](auto &object) { return object.edge_percentage; },
                [](auto &object, auto value) { object.edge_percentage = value; })
            .description("Percentage of edges to load from all available")
            .minimum(0)
            .maximum(1)
            .defaultValue(1);
        builder
            .getset(
                "radius",
                [](auto &object) { return object.radius; },
                [](auto &object, auto value) { object.radius = value; })
            .description("Radius used for the synapse sphere geometry (Ignored for endfeet)")
            .minimum(0.1)
            .defaultValue(2);
        builder
            .getset(
                "edge_report_name",
                [](auto &object) -> auto & { return object.edge_report_name; },
                [](auto &object, auto value) { object.edge_report_name = std::move(value); })
            .description("Name of a synapse report to load along the edge population")
            .required(false);
        return builder.build();
    }
};

template<>
struct JsonAdapter<SonataNodePopulationParameters> : ObjectAdapter<SonataNodePopulationParameters>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("SonataNodePopulationParameters");
        builder
            .getset(
                "node_population",
                [](auto &object) -> auto & { return object.node_population; },
                [](auto &object, auto value) { object.node_population = std::move(value); })
            .description("Name of the node population to load");
        builder
            .getset(
                "node_sets",
                [](auto &object) -> auto & { return object.node_sets; },
                [](auto &object, auto value) { object.node_sets = std::move(value); })
            .description("List of node set names/regex to filter the node population (ignored if node_ids is provided)")
            .required(false);
        builder
            .getset(
                "node_percentage",
                [](auto &object) { return object.node_percentage; },
                [](auto &object, auto value) { object.node_percentage = value; })
            .description("Percentage of nodes to load (after nodeset filter) (ignored if node_ids is provided)")
            .minimum(0)
            .maximum(1)
            .defaultValue(0.01);
        builder
            .getset(
                "node_ids",
                [](auto &object) -> auto & { return object.node_ids; },
                [](auto &object, auto value) { object.node_ids = std::move(value); })
            .description("List of node IDs to load (invalidates 'node_percentage' and 'node_sets')")
            .required(false);
        builder
            .getset(
                "node_count_limit",
                [](auto &object) { return object.node_count_limit; },
                [](auto &object, auto value) { object.node_count_limit = value; })
            .description("Maximum number of nodes to load")
            .required(false);
        builder
            .getset(
                "report_type",
                [](auto &object) { return object.report_type; },
                [](auto &object, auto value) { object.report_type = value; })
            .description("Type of report to load for the given node population")
            .defaultValue(sonataloader::ReportType::None);
        builder
            .getset(
                "report_name",
                [](auto &object) -> auto & { return object.report_name; },
                [](auto &object, auto value) { object.report_name = std::move(value); })
            .description("Name of the report file to load (ignored if report_type is 'none' or 'spikes')")
            .required(false);
        builder
            .getset(
                "spike_transition_time",
                [](auto &object) { return object.spike_transition_time; },
                [](auto &object, auto value) { object.spike_transition_time = value; })
            .description("When loading a spike report, fade-in/out time [ms], from resting to spike state.")
            .minimum(0)
            .defaultValue(1);
        builder
            .getset(
                "edge_populations",
                [](auto &object) -> auto & { return object.edge_populations; },
                [](auto &object, auto value) { object.edge_populations = std::move(value); })
            .description("List of edge populations to load and their settings")
            .required(false);
        builder
            .getset(
                "neuron_morphology_parameters",
                [](auto &object) -> auto & { return object.neuron_morphology_parameters; },
                [](auto &object, auto value) { object.neuron_morphology_parameters = std::move(value); })
            .description("Settings for morphology geometry loading (ignored for vasculature populations)")
            .defaultValue(brayns::JsonFactory::object());
        builder
            .getset(
                "vasculature_geometry_parameters",
                [](auto &object) -> auto & { return object.vasculature_geometry_parameters; },
                [](auto &object, auto value) { object.vasculature_geometry_parameters = std::move(value); })
            .description("Settings for vasculature geometry load (ignored for any non-vasculature population")
            .defaultValue(brayns::JsonFactory::object());
        return builder.build();
    }
};

template<>
struct JsonAdapter<SonataLoaderParameters> : ObjectAdapter<SonataLoaderParameters>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("SonataLoaderParameters");
        builder
            .getset(
                "node_population_settings",
                [](auto &object) -> auto & { return object.node_population_settings; },
                [](auto &object, auto value) { object.node_population_settings = std::move(value); })
            .description("List of node populations to load and their settings");
        return builder.build();
    }
};
} // namespace brayns
