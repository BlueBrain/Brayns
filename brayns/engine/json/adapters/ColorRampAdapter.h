/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#include <brayns/engine/components/ColorRamp.h>

#include <brayns/json/Json.h>

namespace brayns
{
template<>
struct JsonAdapter<ColorRamp> : ObjectAdapter<ColorRamp>
{
    static void reflect()
    {
        title("Bounds");
        getset(
            "range",
            [](auto &object) -> decltype(auto) { return object.getValuesRange(); },
            [](auto &object, const auto &value) { object.setValuesRange(value); })
            .description("Value range")
            .required(false);
        getset(
            "colors",
            [](auto &object) -> decltype(auto) { return object.getColors(); },
            [](auto &object, auto value) { object.setColors(std::move(value)); })
            .description("RGBA colors")
            .required(false);
    }
};
} // namespace brayns
