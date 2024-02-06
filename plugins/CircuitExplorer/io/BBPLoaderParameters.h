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

#include <brayns/json/Json.h>

#include <io/NeuronMorphologyLoaderParameters.h>
#include <io/bbploader/reports/ReportType.h>

#include <optional>

struct BBPLoaderParameters
{
    float percentage = 0;
    std::optional<std::vector<std::string>> targets;
    std::optional<std::vector<uint64_t>> gids;
    bbploader::ReportType report_type = bbploader::ReportType::None;
    std::string report_name;
    float spike_transition_time = 0;
    NeuronMorphologyLoaderParameters neuron_morphology_parameters;
    bool load_afferent_synapses = false;
    bool load_efferent_synapses = false;
};

namespace brayns
{
template<>
struct EnumReflector<bbploader::ReportType>
{
    static EnumMap<bbploader::ReportType> reflect()
    {
        return {
            {"none", bbploader::ReportType::None},
            {"spikes", bbploader::ReportType::Spikes},
            {"compartment", bbploader::ReportType::Compartment}};
    }
};

template<>
struct JsonAdapter<bbploader::ReportType> : EnumAdapter<bbploader::ReportType>
{
};

template<>
struct JsonAdapter<BBPLoaderParameters> : ObjectAdapter<BBPLoaderParameters>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("BBPLoaderParameters");
        builder
            .getset(
                "percentage",
                [](auto &object) { return object.percentage; },
                [](auto &object, auto value) { object.percentage = value; })
            .description("Percentage of neurons to load (Ignored if a list of gids is specified)")
            .minimum(0)
            .maximum(1)
            .defaultValue(0.1);
        builder
            .getset(
                "targets",
                [](auto &object) -> auto & { return object.targets; },
                [](auto &object, auto value) { object.targets = std::move(value); })
            .description("List of targets to load (empty = default target) (Ignored if a list of gids is specified)")
            .required(false);
        builder
            .getset(
                "gids",
                [](auto &object) -> auto & { return object.gids; },
                [](auto &object, auto value) { object.gids = std::move(value); })
            .description("List of GIDs to load, invalidates 'percentage' and 'targets' parameters")
            .required(false);
        builder
            .getset(
                "report_type",
                [](auto &object) { return object.report_type; },
                [](auto &object, auto value) { object.report_type = value; })
            .description("Type of report to load")
            .defaultValue(bbploader::ReportType::None);
        builder
            .getset(
                "report_name",
                [](auto &object) -> auto & { return object.report_name; },
                [](auto &object, auto value) { object.report_name = std::move(value); })
            .description("Name of the report to load, used only if 'report_type' is 'compartment'")
            .required(false);
        builder
            .getset(
                "spike_transition_time",
                [](auto &object) { return object.spike_transition_time; },
                [](auto &object, auto value) { object.spike_transition_time = value; })
            .description("For spike reports, fade-in/out time [ms] from resting to spike state")
            .minimum(0)
            .defaultValue(1);
        builder
            .getset(
                "neuron_morphology_parameters",
                [](auto &object) -> auto & { return object.neuron_morphology_parameters; },
                [](auto &object, auto value) { object.neuron_morphology_parameters = std::move(value); })
            .description("Settings to configure neuron morphology loading")
            .defaultValue(brayns::JsonFactory::object());
        builder
            .getset(
                "load_afferent_synapses",
                [](auto &object) { return object.load_afferent_synapses; },
                [](auto &object, auto value) { object.load_afferent_synapses = value; })
            .description("Wether to add geometry for afferent synapses or not")
            .defaultValue(false);
        builder
            .getset(
                "load_efferent_synapses",
                [](auto &object) { return object.load_efferent_synapses; },
                [](auto &object, auto value) { object.load_efferent_synapses = value; })
            .description("Wether to add geometry for efferent synapses or not")
            .defaultValue(false);
        return builder.build();
    }
};
} // namespace brayns
