/* Copyright 2015-2024 Blue Brain Project/EPFL
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
#include <brayns/utils/StringUtils.h>

#include <plugin/io/NeuronMorphologyLoaderParameters.h>
#include <plugin/io/bbploader/simulation/SimulationType.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_ENUM(bbploader::SimulationType,
                         {"none", bbploader::SimulationType::NONE},
                         {"spikes", bbploader::SimulationType::SPIKES},
                         {"compartment",
                          bbploader::SimulationType::COMPARTMENT})
}

BRAYNS_JSON_OBJECT_BEGIN(BBPLoaderParameters)
BRAYNS_JSON_OBJECT_ENTRY(
    float, percentage,
    "Percentage of neurons to load (Ignored if a list of gids is specified)")
BRAYNS_JSON_OBJECT_ENTRY(
    std::vector<std::string>, targets,
    "List of targets to load. If empty, circuit's default target will be used "
    "(Ignored if a list of gids is specified",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    std::vector<uint64_t>, gids,
    "List of GIDs to load. Invalidates 'percentage' and 'targets' parameters",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    bbploader::SimulationType, report_type,
    "Type of report to load. Possible values are: " +
        string_utils::join(enumNames<bbploader::SimulationType>(), ", "))
BRAYNS_JSON_OBJECT_ENTRY(
    std::string, report_name,
    "Name of the report to load. Used only if 'report_type' is 'compartment'",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    float, spike_transition_time,
    "When loading a spike report, fade-in and fade-out time, in seconds, from "
    "resting state to spike state. Must be greater than or equal to 0.0",
    brayns::Default(1.f))
BRAYNS_JSON_OBJECT_ENTRY(NeuronMorphologyLoaderParameters,
                         neuron_morphology_parameters,
                         "Settings to configure neuron morphology load")
BRAYNS_JSON_OBJECT_ENTRY(bool, load_afferent_synapses,
                         "Wether to add geometry for afferent synapses or not",
                         brayns::Default(false))
BRAYNS_JSON_OBJECT_ENTRY(bool, load_efferent_synapses,
                         "Wether to add geometry for efferent synapses or not",
                         brayns::Default(false))
BRAYNS_JSON_OBJECT_END()
