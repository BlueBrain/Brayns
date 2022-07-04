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
#include <brayns/utils/StringUtils.h>

#include <io/NeuronMorphologyLoaderParameters.h>
#include <io/bbploader/reports/ReportType.h>

#include <optional>

namespace brayns
{
BRAYNS_JSON_ADAPTER_ENUM(
    bbploader::ReportType,
    {"none", bbploader::ReportType::None},
    {"spikes", bbploader::ReportType::Spikes},
    {"compartment", bbploader::ReportType::Compartment})
}

BRAYNS_JSON_OBJECT_BEGIN(BBPLoaderParameters)
BRAYNS_JSON_OBJECT_ENTRY(
    float,
    percentage,
    "Percentage of neurons to load (Ignored if a list of gids is specified)",
    brayns::Minimum(0.001f),
    brayns::Maximum(1.f),
    brayns::Default(0.1f))
BRAYNS_JSON_OBJECT_ENTRY(
    std::optional<std::vector<std::string>>,
    targets,
    "List of targets to load. If empty, circuit's default target will be used "
    "(Ignored if a list of gids is specified",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    std::optional<std::vector<uint64_t>>,
    gids,
    "List of GIDs to load. Invalidates 'percentage' and 'targets' parameters",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_ENTRY(bbploader::ReportType, report_type, "Type of report to load.", brayns::Default("none"))
BRAYNS_JSON_OBJECT_ENTRY(
    std::string,
    report_name,
    "Name of the report to load. Used only if 'report_type' is 'compartment'",
    brayns::Required(false))
BRAYNS_JSON_OBJECT_ENTRY(
    float,
    spike_transition_time,
    "When loading a spike report, fade-in and fade-out time, in milliseconds, from "
    "resting state to spike state.",
    brayns::Default(1.f),
    brayns::Minimum(0.f))
BRAYNS_JSON_OBJECT_ENTRY(
    NeuronMorphologyLoaderParameters,
    neuron_morphology_parameters,
    "Settings to configure neuron morphology load")
BRAYNS_JSON_OBJECT_ENTRY(
    bool,
    load_afferent_synapses,
    "Wether to add geometry for afferent synapses or not",
    brayns::Default(false))
BRAYNS_JSON_OBJECT_ENTRY(
    bool,
    load_efferent_synapses,
    "Wether to add geometry for efferent synapses or not",
    brayns::Default(false))
BRAYNS_JSON_OBJECT_END()
