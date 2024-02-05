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

#include <brayns/json/JsonObjectMacro.h>
#include <brayns/utils/StringUtils.h>

BRAYNS_JSON_OBJECT_BEGIN(NeuronMorphologyLoaderParameters)
BRAYNS_JSON_OBJECT_ENTRY(
    std::string, geometry_mode,
    "Type of geometry to use to display the cells. Possible "
    "values are: 'vanilla', 'samples', 'smooth'",
    brayns::Default("smooth"))
BRAYNS_JSON_OBJECT_ENTRY(
    float, radius_multiplier,
    "Parameter to multiply all morphology sample radii by. "
    "Must be > 0.0. Ignored if 'radius_override' > 0.0",
    brayns::Default(1.f))
BRAYNS_JSON_OBJECT_ENTRY(
    float, radius_override,
    "Parameter to use as radius for all morphology sample radii. 0.0 disables "
    "this parameter. If > 0.0, invalidates 'radius_multiplier'",
    brayns::Default(0.f))
BRAYNS_JSON_OBJECT_ENTRY(bool, load_soma,
                         "Wether to load or not the soma section of the neuron",
                         brayns::Default(true))
BRAYNS_JSON_OBJECT_ENTRY(bool, load_axon,
                         "Wether to load or not the axon section of the neuron",
                         brayns::Default(false))
BRAYNS_JSON_OBJECT_ENTRY(
    bool, load_dendrites,
    "Wether to load or not the dendrite secitons of the neuron",
    brayns::Default(true))
BRAYNS_JSON_OBJECT_END()
