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

#include "JsonAdapter.h"
#include "JsonBuffer.h"
#include "JsonError.h"
#include "JsonSchema.h"
#include "JsonType.h"
#include "JsonValidator.h"

#include "adapters/ArrayAdapter.h"
#include "adapters/EnumAdapter.h"
#include "adapters/MapAdapter.h"
#include "adapters/MathTypesAdapter.h"
#include "adapters/ObjectAdapter.h"
#include "adapters/PrimitiveAdapter.h"
#include "adapters/PtrAdapter.h"
#include "adapters/SchemaAdapter.h"

namespace brayns
{
/**
 * @brief High level class to serialize / deserialize JSON for custom types.
 *
 * Can also be used to build JSON schemas.
 *
 * A valid specialization of JsonAdapter<T> is required for custom types.
 *
 * Example:
 * @code {.cpp}
 * auto test = Json::parse<Test>(json);
 * auto json = Json::stringify(test);
 * @endcode
 *
 */
class Json
{
public:
    /**
     * @brief Convert a JsonValue to string.
     *
     * @param json The JsonValue to stringify.
     * @return std::string A JSON string representing the given value.
     * @throw std::exception JSON cannot be converted to string.
     */
    static std::string stringify(const JsonValue &json);

    /**
     * @brief Parse a JSON string to a JSON value.
     *
     * @param json A JSON string to parse.
     * @return JsonValue The resulting JsonValue
     * @throw JsonParsingException The JSON format is incorrect.
     */
    static JsonValue parse(const std::string &json);

    /**
     * @brief Validate JSON using given schema.
     *
     * @param json JSON to validate.
     * @param schema JSON schema.
     * @return JsonErrors Errors detected during validation (empty if valid).
     */
    static JsonErrors validate(const JsonValue &json, const JsonSchema &schema);

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
        auto json = JsonValue();
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
        auto json = serialize(value);
        return stringify(json);
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
        auto value = T{};
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
     * @param data The JSON string to parse representing the object.
     * @return T The parsed object.
     */
    template<typename T>
    static T parse(const std::string &data)
    {
        auto json = parse(data);
        return deserialize<T>(json);
    }
};
} // namespace brayns
