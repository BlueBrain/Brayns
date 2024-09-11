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
struct JsonConstant
{
    auto operator<=>(const JsonConstant &) const = default;
};

template<typename T>
concept ValidJsonConstant = std::derived_from<T, JsonConstant> && JsonPrimitive<decltype(T::value)>;

template<ValidJsonConstant T>
struct JsonReflector<T>
{
    using Type = decltype(T::value);

    static JsonSchema getSchema()
    {
        return {
            .type = jsonTypeOf<Type>,
            .constant = T::value,
        };
    }

    static void serialize(const T &, JsonValue &json)
    {
        return serializeToJson(T::value, json);
    }

    static void deserialize(const JsonValue &json, T &)
    {
        if (json != T::value)
        {
            throw JsonException("Invalid const");
        }
    }
};

struct JsonFalse : JsonConstant
{
    static constexpr auto value = false;
};
}
