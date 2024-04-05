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

#include <brayns/core/engine/components/Transform.h>

#include <brayns/core/json/Json.h>

namespace brayns
{
template<>
struct JsonAdapter<Transform> : ObjectAdapter<Transform>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Transform");
        builder
            .getset(
                "translation",
                [](auto &object) -> auto & { return object.translation; },
                [](auto &object, const auto &value) { object.translation = value; })
            .description("Translation XYZ")
            .required(false);
        builder
            .getset(
                "rotation",
                [](auto &object) -> auto & { return object.rotation; },
                [](auto &object, const auto &value) { object.rotation = value; })
            .description("Rotation XYZW")
            .required(false);
        builder
            .getset(
                "scale",
                [](auto &object) -> auto & { return object.scale; },
                [](auto &object, const auto &value) { object.scale = value; })
            .description("Scale XYZ")
            .required(false);
        return builder.build();
    }
};
} // namespace brayns
