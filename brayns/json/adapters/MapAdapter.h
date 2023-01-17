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

#include <map>
#include <string>
#include <unordered_map>

#include "PrimitiveAdapter.h"

namespace brayns
{
/**
 * @brief Helper class to serialize map-like types to a JSON object.
 *
 * Map-like means iterable yielding pair<string, T> and operator[string] to
 * emplace values.
 *
 * @tparam T Map-like type.
 */
template<typename T>
struct MapAdapter
{
    using ValueType = typename T::mapped_type;

    /**
     * @brief Create a schema with object type and T::value_type
     * additionalProperties schema.
     *
     * @return JsonSchema T schema.
     */
    static JsonSchema getSchema()
    {
        JsonSchema schema;
        schema.type = JsonType::Object;
        schema.additionalProperties = {Json::getSchema<ValueType>()};
        return schema;
    }

    /**
     * @brief Create a JSON object in json with all key-value pairs from value.
     *
     * @param value Input value.
     * @param json Output JSON.
     */
    static void serialize(const T &value, JsonValue &json)
    {
        auto object = Poco::makeShared<JsonObject>();
        for (const auto &[key, item] : value)
        {
            auto jsonItem = Json::serialize(item);
            object->set(key, jsonItem);
        }
        json = object;
    }

    /**
     * @brief Extract a JsonObject::Ptr from json and serialize in value.
     *
     * @param json Input JSON.
     * @param value Ouput value.
     */
    static void deserialize(const JsonValue &json, T &value)
    {
        auto object = JsonExtractor::extractObject(json);
        if (!object)
        {
            throw std::runtime_error("Not a JSON object");
        }
        value.clear();
        for (const auto &[key, item] : *object)
        {
            Json::deserialize(item, value[key]);
        }
    }
};

/**
 * @brief Partial specialization of JsonAdapter for std::map<std::string, T>.
 *
 * @tparam T Type of the map values.
 */
template<typename T>
struct JsonAdapter<StringMap<T>> : MapAdapter<StringMap<T>>
{
};

/**
 * @brief Partial specialization of JsonAdapter for
 * std::unordered_map<std::string, T>.
 *
 * @tparam T Type of the map values.
 */
template<typename T>
struct JsonAdapter<StringHash<T>> : MapAdapter<StringHash<T>>
{
};
} // namespace brayns
