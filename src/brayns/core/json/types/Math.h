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
    using ValueType = typename T::bound_t;

    static JsonSchema getSchema()
    {
        return {
            .type = JsonType::Array,
            .items = {getJsonSchema<ValueType>()},
            .minItems = 2,
            .maxItems = 2,
        };
    }

    static void serialize(const T &value, JsonValue &json)
    {
        auto array = createJsonArray();

        array->add(serializeToJson(value.lower));
        array->add(serializeToJson(value.upper));

        json = array;
    }

    static void deserialize(const JsonValue &json, T &value)
    {
        const auto &array = getArray(json);

        if (array.size() != 2)
        {
            throw JsonException("Invalid box");
        }

        deserializeJson(array.get(0), value.lower);
        deserializeJson(array.get(1), value.upper);
    }
};

template<typename T>
struct JsonReflector<Range<T>> : JsonBoxReflector<Range<T>>
{
};

template<typename T, int S>
struct JsonReflector<BoxT<T, S>> : JsonBoxReflector<BoxT<T, S>>
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

template<>
struct JsonObjectReflector<Transform2D>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Transform2D>();
        builder.field("translation", [](auto &object) { return &object.translation; }).description("Translation XY").defaultValue(Vector2(0.0F, 0.0F));
        builder.field("rotation", [](auto &object) { return &object.rotation; }).description("Rotation angle in radians").defaultValue(0.0F);
        builder.field("scale", [](auto &object) { return &object.scale; }).description("Scale XY").defaultValue(Vector2(1.0F, 1.0F));
        return builder.build();
    }
};
}
