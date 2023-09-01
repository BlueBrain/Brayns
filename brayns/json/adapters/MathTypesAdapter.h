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

#include <stdexcept>

#include <brayns/utils/MathTypes.h>

#include "PrimitiveAdapter.h"

namespace brayns
{
/**
 * @brief Helper class to serialize math types to fixed-size JSON array.
 *
 * @tparam T math vector type.
 */
template<typename T>
struct MathVectorAdapter
{
    using ValueType = typename T::Scalar;

    static inline constexpr auto itemCount = sizeof(T) / sizeof(typename T::Scalar);

    static JsonSchema getSchema()
    {
        auto schema = JsonSchema();
        schema.type = JsonType::Array;
        schema.items = {JsonAdapter<ValueType>::getSchema()};
        schema.minItems = itemCount;
        schema.maxItems = itemCount;
        return schema;
    }

    static void serialize(const T &value, JsonValue &json)
    {
        auto &array = JsonFactory::emplaceArray(json);

        for (std::size_t i = 0; i < itemCount; ++i)
        {
            auto child = JsonValue();
            JsonAdapter<ValueType>::serialize(value[i], child);
            array.add(child);
        }
    }

    static void deserialize(const JsonValue &json, T &value)
    {
        auto &array = JsonExtractor::extractArray(json);
        if (array.size() != itemCount)
        {
            throw std::invalid_argument("Invalid array size");
        }
        auto i = std::size_t(0);
        for (const auto &item : array)
        {
            JsonAdapter<ValueType>::deserialize(item, value[i]);
            ++i;
        }
    }
};

/**
 * @brief JSON handling for math::vec_t<T, S>.
 *
 * @tparam T Component type.
 * @tparam S Component count.
 */
template<typename T, int S>
struct JsonAdapter<math::vec_t<T, S>> : MathVectorAdapter<math::vec_t<T, S>>
{
};

/**
 * @brief JSON handling for math::QuaternionT<T>.
 *
 * @tparam T Component type.
 */
template<typename T>
struct JsonAdapter<math::QuaternionT<T>>
{
    using ValueType = T;

    static inline constexpr auto itemCount = 4;

    static JsonSchema getSchema()
    {
        auto schema = JsonSchema();
        schema.type = JsonType::Array;
        schema.items = {JsonAdapter<ValueType>::getSchema()};
        schema.minItems = itemCount;
        schema.maxItems = itemCount;
        return schema;
    }

    static void serialize(const math::QuaternionT<T> &value, JsonValue &json)
    {
        auto &array = JsonFactory::emplaceArray(json);

        auto components = &value.i;

        for (std::size_t i = 0; i < itemCount; ++i)
        {
            auto child = JsonValue();
            JsonAdapter<ValueType>::serialize(components[i], child);
            array.add(child);
        }
    }

    static void deserialize(const JsonValue &json, math::QuaternionT<T> &value)
    {
        auto &array = JsonExtractor::extractArray(json);
        if (array.size() != itemCount)
        {
            throw std::invalid_argument("Invalid array size");
        }

        auto components = &value.i;
        auto i = std::size_t(0);
        for (const auto &item : array)
        {
            JsonAdapter<ValueType>::deserialize(item, components[i++]);
        }
    }
};
} // namespace brayns
