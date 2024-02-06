/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <brayns/parameters/SimulationParameters.h>

namespace brayns
{
template<>
struct JsonAdapter<SimulationParameters> : ObjectAdapter<SimulationParameters>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("SimulationParameters");
        builder.get("start_frame", [](auto &object) { return object.getStartFrame(); })
            .description("Initial simulation frame index");
        builder.get("end_frame", [](auto &object) { return object.getEndFrame(); })
            .description("Final simulation frame index");
        builder
            .getset(
                "current",
                [](auto &object) { return object.getFrame(); },
                [](auto &object, auto value) { object.setFrame(value); })
            .description("Current simulation frame index")
            .required(false);
        builder.get("dt", [](auto &object) { return object.getDt(); }).description("Delta time between two frames");
        builder
            .get(
                "unit",
                [](auto &object) -> auto & { return object.getTimeUnit(); })
            .description("Time unit");
        return builder.build();
    }
};
} // namespace brayns
