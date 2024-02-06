/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <brayns/engine/model/systemtypes/ColorSystem.h>

namespace brayns
{
struct ColorMethodValuesMessage
{
    uint32_t id = 0;
    std::string method;
};

template<>
struct JsonAdapter<ColorMethodValuesMessage> : ObjectAdapter<ColorMethodValuesMessage>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("ColorMethodValuesMessage");
        builder
            .getset(
                "id",
                [](auto &object) { return object.id; },
                [](auto &object, auto value) { object.id = value; })
            .description("ID of the model that will be colored");
        builder
            .getset(
                "method",
                [](auto &object) -> auto & { return object.method; },
                [](auto &object, auto value) { object.method = std::move(value); })
            .description("Coloring method which values will be returned");
        return builder.build();
    }
};

struct ColorModelMessage
{
    uint32_t id = 0;
    std::string method;
    ColorMethodInput values;
};

template<>
struct JsonAdapter<ColorModelMessage> : ObjectAdapter<ColorModelMessage>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("ColorModelMessage");
        builder
            .getset(
                "id",
                [](auto &object) { return object.id; },
                [](auto &object, auto value) { object.id = value; })
            .description("ID of the model to color");
        builder
            .getset(
                "method",
                [](auto &object) -> auto & { return object.method; },
                [](auto &object, auto value) { object.method = std::move(value); })
            .description("Coloring method");
        builder
            .getset(
                "values",
                [](auto &object) -> auto & { return object.values; },
                [](auto &object, auto value) { object.values = std::move(value); })
            .description("Coloring parameters");
        return builder.build();
    }
};
} // namespace brayns
