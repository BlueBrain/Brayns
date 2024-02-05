/* Copyright 2015-2024 Blue Brain Project/EPFL
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
template <typename T>
struct MapAdapter
{
    using ValueType = typename T::mapped_type;

    /**
     * @brief Create a schema with object type and T::value_type
     * additionalProperties schema.
     *
     * @return JsonSchema T schema.
     */
    static JsonSchema getSchema(const T&)
    {
        JsonSchema schema;
        schema.type = JsonType::Object;
        schema.additionalProperties = {Json::getSchema<ValueType>()};
        return schema;
    }

    /**
     * @brief Create a JSON object in json with all key-value pairs from value.
     *
     * @param value Map-like object to serialize.
     * @param json Output JSON.
     * @return true Success.
     * @return false Failure.
     */
    static bool serialize(const T& value, JsonValue& json)
    {
        auto object = Poco::makeShared<JsonObject>();
        for (const auto& pair : value)
        {
            JsonValue jsonItem;
            if (!Json::serialize(pair.second, jsonItem))
            {
                return false;
            }
            object->set(pair.first, jsonItem);
        }
        json = object;
        return true;
    }

    /**
     * @brief Extract a JsonObject::Ptr from json and serialize in value.
     *
     * If successful, value will contains all key-value pairs from the JSON
     * object, otherwise it will be unchanged.
     *
     * @param json Input JSON.
     * @param value Ouput map-like object.
     * @return true Success.
     * @return false Failure.
     */
    static bool deserialize(const JsonValue& json, T& value)
    {
        auto object = JsonExtractor::extractObject(json);
        if (!object)
        {
            return false;
        }
        T buffer;
        for (const auto& pair : *object)
        {
            if (!Json::deserialize(pair.second, buffer[pair.first]))
            {
                return false;
            }
        }
        value = std::move(buffer);
        return true;
    }
};

/**
 * @brief Partial specialization of JsonAdapter for std::map<std::string, T>.
 *
 * @tparam T Type of the map values.
 */
template <typename T>
struct JsonAdapter<StringMap<T>> : MapAdapter<StringMap<T>>
{
};

/**
 * @brief Partial specialization of JsonAdapter for
 * std::unordered_map<std::string, T>.
 *
 * @tparam T Type of the map values.
 */
template <typename T>
struct JsonAdapter<StringHash<T>> : MapAdapter<StringHash<T>>
{
};
} // namespace brayns