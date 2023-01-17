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

#include <deque>
#include <list>
#include <vector>

#include "PrimitiveAdapter.h"

namespace brayns
{
/**
 * @brief Helper class to manipulate JSON for array-like types.
 *
 * Array like type can be constructed with a size_t and iterated using begin and
 * end.
 *
 * @tparam T The type of the array container.
 */
template<typename T>
struct ArrayAdapter
{
    using ValueType = typename T::value_type;

    /**
     * @brief Create a schema of array type and reflected item type.
     *
     * @return JsonSchema Json schema of the container T.
     */
    static JsonSchema getSchema()
    {
        JsonSchema schema;
        schema.type = JsonType::Array;
        schema.items = {Json::getSchema<ValueType>()};
        return schema;
    }

    /**
     * @brief Serialize value in json.
     *
     * Iterate over value using begin(value) and end(value) to fill a JSON array
     * with contained item (must be serializable).
     *
     * @param value Input value.
     * @param json Output JSON.
     */
    static void serialize(const T &value, JsonValue &json)
    {
        auto array = Poco::makeShared<JsonArray>();
        for (const auto &item : value)
        {
            auto jsonItem = Json::serialize(item);
            array->add(jsonItem);
        }
        json = array;
    }

    /**
     * @brief Deserialize json in value.
     *
     * Extract a json array from json and deserialize elements in value.
     *
     * @param json Input JSON.
     * @param value Output value.
     */
    static void deserialize(const JsonValue &json, T &value)
    {
        auto array = JsonExtractor::extractArray(json);
        if (!array)
        {
            throw std::runtime_error("Not an array");
        }
        value.clear();
        for (const auto &jsonItem : *array)
        {
            auto &item = value.emplace_back();
            Json::deserialize(jsonItem, item);
        }
    }
};

/**
 * @brief Specialization for boolean ref of std::vector<bool>.
 *
 */
template<>
struct JsonAdapter<std::vector<bool>::reference>
{
    using Ref = std::vector<bool>::reference;

    /**
     * @brief Get schema as boolean.
     *
     * @param value Value to get the schema from.
     * @return JsonSchema Schema of value.
     */
    static JsonSchema getSchema()
    {
        return Json::getSchema<bool>();
    }

    /**
     * @brief Serialize value as boolean
     *
     * @param value Input value.
     * @param json Output JSON.
     */
    static void serialize(Ref value, JsonValue &json)
    {
        json = static_cast<bool>(value);
    }

    /**
     * @brief Deserialize boolean JSON into value.
     *
     * @param json Input JSON.
     * @param value Ouput value.
     */
    static void deserialize(const JsonValue &json, Ref value)
    {
        value = Json::deserialize<bool>(json);
    }
};

/**
 * @brief Specialization of JsonAdapter for vector<T>.
 *
 * @tparam T Item type.
 */
template<typename T>
struct JsonAdapter<std::vector<T>> : ArrayAdapter<std::vector<T>>
{
};

/**
 * @brief Specialization of JsonAdapter for deque<T>.
 *
 * @tparam T Item type.
 */
template<typename T>
struct JsonAdapter<std::deque<T>> : ArrayAdapter<std::deque<T>>
{
};

/**
 * @brief Specialization of JsonAdapter for list<T>.
 *
 * @tparam T Item type.
 */
template<typename T>
struct JsonAdapter<std::list<T>> : ArrayAdapter<std::list<T>>
{
};
} // namespace brayns
