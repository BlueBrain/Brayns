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

#include <map>
#include <string>
#include <type_traits>
#include <unordered_map>

#include <Poco/JSON/Array.h>
#include <Poco/JSON/Object.h>

#include <brayns/utils/EnumInfo.h>

namespace brayns
{
/**
 * @brief JSON value (object, array, string, number, bool, null).
 *
 */
using JsonValue = Poco::Dynamic::Var;

/**
 * @brief JSON array (vector of JsonValue).
 *
 */
using JsonArray = Poco::JSON::Array;

/**
 * @brief JSON object (map of string to JsonValue).
 *
 */
using JsonObject = Poco::JSON::Object;

/**
 * @brief Shorcut for map<string, T> to avoid commas in macros.
 *
 * @tparam T Map value type.
 */
template<typename T>
using StringMap = std::map<std::string, T>;

/**
 * @brief Shorcut for unordered_map<string, T> to avoid commas in macros.
 *
 * @tparam T Map value type.
 */
template<typename T>
using StringHash = std::unordered_map<std::string, T>;

/**
 * @brief All available JSON types.
 *
 */
enum class JsonType
{
    Undefined,
    Null,
    Boolean,
    Integer,
    Number,
    String,
    Array,
    Object
};

template<>
struct EnumReflector<JsonType>
{
    static EnumMap<JsonType> reflect()
    {
        return {
            {"undefined", JsonType::Undefined},
            {"null", JsonType::Null},
            {"boolean", JsonType::Boolean},
            {"integer", JsonType::Integer},
            {"number", JsonType::Number},
            {"string", JsonType::String},
            {"array", JsonType::Array},
            {"object", JsonType::Object}};
    }
};

/**
 * @brief Helper class to get info about JSON type.
 *
 */
struct JsonTypeHelper
{
    /**
     * @brief Check if the given type is accepted by required.
     *
     * @param required Required type.
     * @param type Type to check.
     * @return true Type is accepted by required.
     * @return false Type is not accepted by required.
     */
    static bool check(JsonType required, JsonType type);

    /**
     * @brief Check if the given type is numeric (number or integer).
     *
     * @param type Type to check.
     * @return true Type is numeric.
     * @return false Type is not numeric.
     */
    static bool isNumeric(JsonType type);
};

/**
 * @brief Helper type to get JSON type.
 *
 */
struct GetJsonType
{
    /**
     * @brief Return the JSON type from a JSON value.
     *
     * @param json JSON to evaluate.
     * @return JsonType JSON type.
     */
    static JsonType fromJson(const JsonValue &json);

    /**
     * @brief Return the JSON type from a primitive type.
     *
     * @tparam T Primitive type to check.
     * @return JsonType JSON type.
     */
    template<typename T>
    static constexpr JsonType fromPrimitive()
    {
        if (std::is_same<T, nullptr_t>())
        {
            return JsonType::Null;
        }
        if (std::is_same<T, bool>())
        {
            return JsonType::Boolean;
        }
        if (std::is_integral<T>())
        {
            return JsonType::Integer;
        }
        if (std::is_arithmetic<T>())
        {
            return JsonType::Number;
        }
        if (std::is_same<T, std::string>())
        {
            return JsonType::String;
        }
        return JsonType::Undefined;
    }
};
} // namespace brayns
