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

#include <sstream>

#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Stringifier.h>

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
    static std::string stringify(const JsonValue& json)
    {
        std::ostringstream stream;
        Poco::JSON::Stringifier::condense(json, stream);
        return stream.str();
    }

    /**
     * @brief Parse a JSON string to a JSON value.
     *
     * @param json A JSON string to parse.
     * @return JsonValue The resulting JsonValue
     * @throw Poco::JSON::Exception The JSON format is incorrect.
     */
    static JsonValue parse(const std::string& json)
    {
        Poco::JSON::Parser parser;
        return parser.parse(json);
    }

    /**
     * @brief Return the JSON schema of value using JsonAdapter<T>.
     *
     * @tparam T Type to get the schema from.
     * @param value Value used to build the schema.
     * @return const JsonSchema& JSON schema of T.
     */
    template <typename T>
    static JsonSchema getSchema(const T& value)
    {
        return JsonAdapter<T>::getSchema(value);
    }

    /**
     * @brief Shortcut to get the schema if no object instance is available.
     *
     * @tparam T Type to get the schema from.
     * @return const JsonSchema& JSON schema of T.
     */
    template <typename T>
    static JsonSchema getSchema()
    {
        static const T value = {};
        return getSchema(value);
    }

    /**
     * @brief Serialize a given type to a JsonValue using the
     * JsonAdapter<T>::serialize (must have a valid specialization).
     *
     * @tparam T Type of the object to serialize.
     * @param value Object to serialize.
     * @return JsonValue JSON source.
     */
    template <typename T>
    static JsonValue serialize(const T& value)
    {
        JsonValue json;
        JsonAdapter<T>::serialize(value, json);
        return json;
    }

    /**
     * @brief Serialize with default value.
     *
     * @tparam T Type of the input object.
     * @param json JsonValue to serialize.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    template <typename T>
    static bool serialize(const T& value, JsonValue& json)
    {
        return JsonAdapter<T>::serialize(value, json);
    }

    /**
     * @brief Convert a custom object to JSON string using serialize and
     * stringify.
     *
     * @tparam T The type of the object to serialize.
     * @param value The object to serialize.
     * @return std::string JSON string representing the object.
     */
    template <typename T>
    static std::string stringify(const T& value)
    {
        return stringify(serialize(value));
    }

    /**
     * @brief Convert a JsonValue to a given type using
     * JsonAdapter<T>::serialize (must have a valid specialization).
     *
     * @tparam T Type of the resulting object.
     * @param json JsonValue containing the object value.
     * @return T Deserialized object.
     */
    template <typename T>
    static T deserialize(const JsonValue& json)
    {
        T value{};
        JsonAdapter<T>::deserialize(json, value);
        return value;
    }

    /**
     * @brief Deserialize with default value.
     *
     * @tparam T Type of the resulting object.
     * @param json JsonValue containing the object value.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    template <typename T>
    static bool deserialize(const JsonValue& json, T& value)
    {
        return JsonAdapter<T>::deserialize(json, value);
    }

    /**
     * @brief Parse a JSON string to a given type using deserialize and
     * parse.
     *
     * @tparam T The resulting object type.
     * @param json The JSON string to parse representing the object.
     * @return T The parsed object.
     */
    template <typename T>
    static T parse(const std::string& json)
    {
        return deserialize<T>(parse(json));
    }
};

/**
 * @brief Specialization to do nothing if value is already JSON.
 *
 */
template <>
struct JsonAdapter<JsonValue>
{
    /**
     * @brief Create an empty schema.
     *
     * @return JsonSchema Empty schema.
     */
    static JsonSchema getSchema(const JsonValue&) { return {}; }

    /**
     * @brief Copy value into json.
     *
     * @param value The input JSON.
     * @param json The output JSON.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool serialize(const JsonValue& value, JsonValue& json)
    {
        json = value;
        return true;
    }

    /**
     * @brief Copy json into value.
     *
     * @param json The input JSON.
     * @param value The output JSON.
     * @return true if success, false if failure, the output value is left
     * unchanged in this case.
     */
    static bool deserialize(const JsonValue& json, JsonValue& value)
    {
        value = json;
        return true;
    }
};
} // namespace brayns