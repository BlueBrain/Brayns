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

struct DTILoaderParameters
{
    float radius = 0;
    float spike_decay_time = 0;
};

namespace brayns
{
template<>
struct JsonAdapter<DTILoaderParameters> : ObjectAdapter<DTILoaderParameters>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("DTILoaderParameters");
        builder
            .getset(
                "radius",
                [](auto &object) { return object.radius; },
                [](auto &object, auto value) { object.radius = value; })
            .description("Connectivity streamlines radius");
        builder
            .getset(
                "spike_decay_time",
                [](auto &object) { return object.spike_decay_time; },
                [](auto &object, auto value) { object.spike_decay_time = value; })
            .description("Time [ms], that a spikes takes to go from the beginning to the end of the streamline")
            .minimum(0)
            .defaultValue(1);
        return builder.build();
    }
};
} // namespace brayns
