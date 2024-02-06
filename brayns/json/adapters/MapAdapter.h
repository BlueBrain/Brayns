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

#include <map>
#include <string>
#include <unordered_map>

#include "PrimitiveAdapter.h"

namespace brayns
{
/**
 * @brief Helper class to serialize map-like types to a JSON object.
 *
 * Map types have methods begin(), end() and operator[](const std::string &).
 *
 * @tparam T Map-like type.
 */
template<typename T>
struct MapAdapter
{
    using ValueType = typename T::mapped_type;

    static JsonSchema getSchema()
    {
        auto schema = JsonSchema();
        schema.type = JsonType::Object;
        schema.items = {JsonAdapter<ValueType>::getSchema()};
        return schema;
    }

    static void serialize(const T &value, JsonValue &json)
    {
        auto &object = JsonFactory::emplaceObject(json);
        for (const auto &[key, item] : value)
        {
            auto child = JsonValue();
            JsonAdapter<ValueType>::serialize(item, child);
            object.set(key, child);
        }
    }

    static void deserialize(const JsonValue &json, T &value)
    {
        auto &object = JsonExtractor::extractObject(json);
        value.clear();
        for (const auto &[key, child] : object)
        {
            auto &item = value[key];
            JsonAdapter<ValueType>::deserialize(child, item);
        }
    }
};

/**
 * @brief JSON handling for std::map<std::string, T>.
 *
 * @tparam T Value type.
 */
template<typename T>
struct JsonAdapter<std::map<std::string, T>> : MapAdapter<std::map<std::string, T>>
{
};

/**
 * @brief JSON handling for std::unordered_map<std::string, T>.
 *
 * @tparam T Value type.
 */
template<typename T>
struct JsonAdapter<std::unordered_map<std::string, T>> : MapAdapter<std::unordered_map<std::string, T>>
{
};
} // namespace brayns
