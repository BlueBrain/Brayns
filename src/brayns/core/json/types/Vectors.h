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

#include "Primitives.h"

namespace brayns
{
template<typename T>
struct JsonMathReflector
{
    using ValueType = typename T::Scalar;

    static inline constexpr auto itemCount = sizeof(T) / sizeof(ValueType);

    static JsonSchema getSchema()
    {
        return {
            .type = JsonType::Array,
            .items = {getJsonSchema<ValueType>()},
            .minItems = itemCount,
            .maxItems = itemCount,
        };
    }

    static JsonValue serialize(const T &value)
    {
        auto array = createJsonArray();

        for (auto i = std::size_t(0); i < itemCount; ++i)
        {
            const auto &item = getItem(value, i);
            auto jsonItem = serializeToJson(item);

            array->add(jsonItem);
        }
        return array;
    }

    static T deserialize(const JsonValue &json)
    {
        const auto &array = getArray(json);
        auto value = T();

        if (array.size() != itemCount)
        {
            throw JsonException("Invalid static array size");
        }

        auto i = std::size_t(0);

        for (const auto &jsonItem : array)
        {
            auto &item = getItem(value, i);

            item = deserializeAs<ValueType>(jsonItem);

            ++i;
        }
        return value;
    }

private:
    static auto &getItem(auto &value, std::size_t index)
    {
        return value[index];
    }

    static auto &getItem(const Quaternion &value, std::size_t index)
    {
        return (&value.i)[index];
    }

    static auto &getItem(Quaternion &value, std::size_t index)
    {
        return (&value.i)[index];
    }
};

template<typename T, int S>
struct JsonReflector<Vector<T, S>> : JsonMathReflector<Vector<T, S>>
{
};

template<>
struct JsonReflector<Quaternion> : JsonMathReflector<Quaternion>
{
};
}
