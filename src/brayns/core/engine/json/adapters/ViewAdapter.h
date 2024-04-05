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

#include <brayns/core/engine/camera/View.h>

#include <brayns/core/json/Json.h>

namespace brayns
{
template<>
struct JsonAdapter<View> : ObjectAdapter<View>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("View");
        builder
            .getset(
                "position",
                [](auto &object) -> auto & { return object.position; },
                [](auto &object, const auto &value) { object.position = value; })
            .description("Camera position XYZ");
        builder
            .getset(
                "target",
                [](auto &object) -> auto & { return object.target; },
                [](auto &object, const auto &value) { object.target = value; })
            .description("Camera target XYZ");
        builder
            .getset(
                "up",
                [](auto &object) -> auto & { return object.up; },
                [](auto &object, const auto &value) { object.up = value; })
            .description("Camera up vector XYZ");
        return builder.build();
    }
};
} // namespace brayns
