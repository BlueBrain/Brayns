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

#include <deque>
#include <list>
#include <vector>

#include "PrimitiveAdapter.h"

namespace brayns
{
/**
 * @brief Helper class to manipulate JSON for array-like types.
 *
 * Array like have methods clear(), emplace_back(), begin() and end().
 *
 * @tparam T Container type.
 */
template<typename T>
struct ArrayAdapter
{
    using ValueType = typename T::value_type;

    static JsonSchema getSchema()
    {
        auto schema = JsonSchema();
        schema.type = JsonType::Array;
        schema.items = {JsonAdapter<ValueType>::getSchema()};
        return schema;
    }

    static void serialize(const T &value, JsonValue &json)
    {
        auto &array = JsonFactory::emplaceArray(json);
        for (const auto &item : value)
        {
            auto child = JsonValue();
            JsonAdapter<ValueType>::serialize(item, child);
            array.add(child);
        }
    }

    static void deserialize(const JsonValue &json, T &value)
    {
        auto &array = JsonExtractor::extractArray(json);
        value.clear();
        for (const auto &child : array)
        {
            auto &item = value.emplace_back();
            JsonAdapter<ValueType>::deserialize(child, item);
        }
    }
};

/**
 * @brief JSON handling for std::vector<bool>::reference.
 *
 */
template<>
struct JsonAdapter<std::vector<bool>::reference>
{
    using Ref = std::vector<bool>::reference;

    static JsonSchema getSchema()
    {
        return JsonAdapter<bool>::getSchema();
    }

    static void serialize(Ref value, JsonValue &json)
    {
        json = static_cast<bool>(value);
    }

    static void deserialize(const JsonValue &json, Ref value)
    {
        value = json.convert<bool>();
    }
};

/**
 * @brief JSON handling for std::vector<T>.
 *
 * @tparam T Item type.
 */
template<typename T>
struct JsonAdapter<std::vector<T>> : ArrayAdapter<std::vector<T>>
{
};

/**
 * @brief JSON handling for std::deque<T>.
 *
 * @tparam T Item type.
 */
template<typename T>
struct JsonAdapter<std::deque<T>> : ArrayAdapter<std::deque<T>>
{
};

/**
 * @brief JSON handling for std::list<T>.
 *
 * @tparam T Item type.
 */
template<typename T>
struct JsonAdapter<std::list<T>> : ArrayAdapter<std::list<T>>
{
};
} // namespace brayns
