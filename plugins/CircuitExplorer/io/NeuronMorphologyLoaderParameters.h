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

#include <brayns/json/JsonObjectMacro.h>
#include <brayns/utils/StringUtils.h>

BRAYNS_JSON_OBJECT_BEGIN(NeuronMorphologyLoaderParameters)
BRAYNS_JSON_OBJECT_ENTRY(
    float,
    radius_multiplier,
    "Parameter to multiply all morphology sample radii by",
    brayns::Default(1.f),
    brayns::Minimum(0.1f))
BRAYNS_JSON_OBJECT_ENTRY(bool, load_soma, "Load the soma section of the neuron", brayns::Default(true))
BRAYNS_JSON_OBJECT_ENTRY(bool, load_axon, "Load the axon section of the neuron", brayns::Default(false))
BRAYNS_JSON_OBJECT_ENTRY(bool, load_dendrites, "Load the dendrite secitons of the neuron", brayns::Default(true))
BRAYNS_JSON_OBJECT_END()
