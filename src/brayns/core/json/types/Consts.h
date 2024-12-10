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

#include <concepts>

#include "Primitives.h"

namespace brayns
{
template<typename T>
struct JsonConstReflector;

template<typename T>
concept ReflectedJsonConst = requires {
    { JsonConstReflector<T>::reflect() } -> ReflectedJson;
};

template<ReflectedJsonConst T>
auto getJsonConst()
{
    return JsonConstReflector<T>::reflect();
}

template<ReflectedJson T>
JsonSchema createJsonConstSchema(T value)
{
    return {
        .type = jsonTypeOf<decltype(value)>,
        .constant = std::move(value),
    };
}

template<ReflectedJson T>
void deserializeJsonConst(const JsonValue &json, const T &value)
{
    if (deserializeJsonAs<T>(json) != value)
    {
        throw JsonException("Invalid const");
    }
}

template<ReflectedJsonConst T>
struct JsonReflector<T>
{
    static inline const auto value = getJsonConst<T>();

    static JsonSchema getSchema()
    {
        return createJsonConstSchema(value);
    }

    static void serialize(const T &, JsonValue &json)
    {
        return serializeToJson(value, json);
    }

    static void deserialize(const JsonValue &json, T &)
    {
        deserializeJsonConst(json, value);
    }
};

struct JsonFalse
{
    auto operator<=>(const JsonFalse &) const = default;
};

template<>
struct JsonConstReflector<JsonFalse>
{
    static bool reflect()
    {
        return false;
    }
};
}
