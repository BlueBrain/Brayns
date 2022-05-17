/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "JsonAdapter.h"
#include "JsonType.h"

namespace brayns
{
/**
 * @brief High level class to serialize / deserialize JSON for custom types.
 *
 * Can also be used to build JSON schemas.
 *
 * A valid specialization of JsonAdapter<T> is required to allow this usage.
 *
 * Example:
 * @code {.cpp}
 * auto test = Json::parse<Test>(json);
 * auto json = Json::stringify(test);
 * @endcode
 *
 */
struct Json
{
    /**
     * @brief Convert a JsonValue to string.
     *
     * @param json The JsonValue to stringify.
     * @return std::string A JSON string representing the given value.
     * @throw Poco::JSON::Exception The JsonValue doesn't contain JSON
     * values.
     */
    static std::string stringify(const JsonValue &json);

    /**
     * @brief Parse a JSON string to a JSON value.
     *
     * @param json A JSON string to parse.
     * @return JsonValue The resulting JsonValue
     * @throw Poco::JSON::Exception The JSON format is incorrect.
     */
    static JsonValue parse(const std::string &json);

    /**
     * @brief Return the JSON schema of T using JsonAdapter<T>::getSchema().
     *
     * @tparam T Type to get the schema from.
     * @return JsonSchema JSON schema of T.
     */
    template<typename T>
    static JsonSchema getSchema()
    {
        return JsonAdapter<T>::getSchema();
    }

    /**
     * @brief Serialize a given type to a JsonValue using the
     * JsonAdapter<T>::serialize.
     *
     * @tparam T Input value type.
     * @param value Input value.
     * @return JsonValue Output JSON.
     */
    template<typename T>
    static JsonValue serialize(const T &value)
    {
        JsonValue json;
        serialize(value, json);
        return json;
    }

    /**
     * @brief Serialize with default value.
     *
     * @tparam T Type of the input object.
     * @param value Input value.
     * @param json Output JSON.
     */
    template<typename T>
    static void serialize(const T &value, JsonValue &json)
    {
        JsonAdapter<T>::serialize(value, json);
    }

    /**
     * @brief Convert a custom object to JSON string using serialize and
     * stringify.
     *
     * @tparam T Input value type.
     * @param value Input value.
     * @return std::string Output JSON string.
     */
    template<typename T>
    static std::string stringify(const T &value)
    {
        return stringify(serialize(value));
    }

    /**
     * @brief Convert a JsonValue to a given type using
     * JsonAdapter<T>::serialize.
     *
     * @tparam T Output value type.
     * @param json Input JSON.
     * @return T Output value.
     */
    template<typename T>
    static T deserialize(const JsonValue &json)
    {
        T value{};
        deserialize(json, value);
        return value;
    }

    /**
     * @brief Deserialize with default value.
     *
     * @tparam T Type of the resulting object.
     * @param json Input JSON.
     * @param value Ouput object.
     */
    template<typename T>
    static void deserialize(const JsonValue &json, T &value)
    {
        JsonAdapter<T>::deserialize(json, value);
    }

    /**
     * @brief Parse a JSON string to a given type using deserialize and
     * parse.
     *
     * @tparam T The resulting object type.
     * @param json The JSON string to parse representing the object.
     * @return T The parsed object.
     */
    template<typename T>
    static T parse(const std::string &json)
    {
        return deserialize<T>(parse(json));
    }
};

/**
 * @brief Specialization to do nothing if value is already JSON.
 *
 */
template<>
struct JsonAdapter<JsonValue>
{
    /**
     * @brief Create an empty schema.
     *
     * @return JsonSchema Empty schema.
     */
    static JsonSchema getSchema();

    /**
     * @brief Copy value into json.
     *
     * @param value The input JSON.
     * @param json The output JSON.
     */
    static void serialize(const JsonValue &value, JsonValue &json);

    /**
     * @brief Copy json into value.
     *
     * @param json The input JSON.
     * @param value The output JSON.
     */
    static void deserialize(const JsonValue &json, JsonValue &value);
};
} // namespace brayns
