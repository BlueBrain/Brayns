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

namespace brayns
{
struct CameraRegionMessage
{
    Vector2f image_start = {1, 0};
    Vector2f image_end = {0, 1};
};

template<>
struct JsonAdapter<CameraRegionMessage> : ObjectAdapter<CameraRegionMessage>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("CameraRegionMessage");
        builder
            .getset(
                "image_start",
                [](auto &object) { return object.image_start; },
                [](auto &object, auto value) { object.image_start = value; })
            .description("Camera image region lower bound XY normalized");
        builder
            .getset(
                "image_end",
                [](auto &object) { return object.image_end; },
                [](auto &object, auto value) { object.image_end = value; })
            .description("Camera image region upper bound XY normalized");
        return builder.build();
    }
};
} // namespace brayns
