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

#include <limits>
#include <type_traits>

#include <brayns/core/json/JsonReflector.h>

namespace brayns
{
template<typename T>
concept JsonPrimitive = isPrimitive(jsonTypeOf<T>);

template<JsonPrimitive T>
struct JsonReflector<T>
{
    static JsonSchema getSchema()
    {
        constexpr auto type = jsonTypeOf<T>;

        auto schema = JsonSchema{.type = type};

        if constexpr (isNumeric(type))
        {
            schema.minimum = std::numeric_limits<T>::lowest();
            schema.maximum = std::numeric_limits<T>::max();
        }

        return schema;
    }

    static void serialize(const T &value, JsonValue &json)
    {
        if constexpr (std::is_same_v<T, NullJson>)
        {
            json.clear();
        }
        else
        {
            json = value;
        }
    }

    static void deserialize(const JsonValue &json, T &value)
    {
        throwIfNotCompatible<T>(json);

        if constexpr (std::is_same_v<T, JsonValue>)
        {
            value = json;
        }
        else if constexpr (std::is_same_v<T, NullJson>)
        {
            value = {};
        }
        else
        {
            value = json.convert<T>();
        }
    }
};
}
