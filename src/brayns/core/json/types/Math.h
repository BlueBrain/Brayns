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

#include <brayns/core/utils/Math.h>

#include "Objects.h"
#include "Vectors.h"

namespace brayns
{
template<typename T>
struct JsonBoxReflector
{
    static auto reflect()
    {
        auto builder = JsonBuilder<T>();
        builder.field("min", [](auto &object) { return &object.lower; }).description("Lower bounds");
        builder.field("max", [](auto &object) { return &object.upper; }).description("Upper bounds");
        return builder.build();
    }
};

template<typename T>
struct JsonObjectReflector<Range<T>> : JsonBoxReflector<Range<T>>
{
};

template<typename T, int S>
struct JsonObjectReflector<BoxT<T, S>> : JsonBoxReflector<BoxT<T, S>>
{
};

template<>
struct JsonObjectReflector<Transform>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Transform>();
        builder.field("translation", [](auto &object) { return &object.translation; })
            .description("Translation XYZ")
            .defaultValue(Vector3(0.0F, 0.0F, 0.0F));
        builder.field("rotation", [](auto &object) { return &object.rotation; })
            .description("Rotation quaternion XYZW")
            .defaultValue(Quaternion(1.0F, 0.0F, 0.0F, 0.0F));
        builder.field("scale", [](auto &object) { return &object.scale; }).description("Scale XYZ").defaultValue(Vector3(1.0F, 1.0F, 1.0F));
        return builder.build();
    }
};
}
