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
    Unknown,
    Null,
    Boolean,
    Integer,
    Number,
    String,
    Array,
    Object
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

    /**
     * @brief Check if the type is primitive (not object nor array).
     *
     * @param type Type to check.
     * @return true Type is primitive.
     * @return false Type is object or array.
     */
    static bool isPrimitive(JsonType type);

    /**
     * @brief Check if the given JSON is a primitive.
     *
     * @param json JSON to check.
     * @return true JSON is primitive.
     * @return false JSON is not primitive.
     */
    static bool isPrimitive(const JsonValue &json);

    /**
     * @brief Check if the type is primitive.
     *
     * @tparam T Type to check.
     * @return true Type is primitive.
     * @return false Type is object or array.
     */
    template<typename T>
    static constexpr bool isPrimitive()
    {
        return std::is_arithmetic<T>() || std::is_same<T, std::string>();
    }
};

/**
 * @brief JSON type name storage.
 *
 */
class JsonTypeName
{
public:
    /**
     * @brief Null type name.
     *
     * @return const std::string& Type name.
     */
    static const std::string &ofNull();

    /**
     * @brief Boolean type name.
     *
     * @return const std::string& Type name.
     */
    static const std::string &ofBoolean();

    /**
     * @brief Integer type name.
     *
     * @return const std::string& Type name.
     */
    static const std::string &ofInteger();

    /**
     * @brief Number type name.
     *
     * @return const std::string& Type name.
     */
    static const std::string &ofNumber();

    /**
     * @brief String type name.
     *
     * @return const std::string& Type name.
     */
    static const std::string &ofString();

    /**
     * @brief Array type name.
     *
     * @return const std::string& Type name.
     */
    static const std::string &ofArray();

    /**
     * @brief Object type name.
     *
     * @return const std::string& Type name.
     */
    static const std::string &ofObject();
};

/**
 * @brief Helper type to get JSON type.
 *
 */
struct GetJsonType
{
    /**
     * @brief Return the JSON type from its name.
     *
     * @param name Type name.
     * @return JsonType JSON type.
     */
    static JsonType fromName(const std::string &name);

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
        return JsonType::Unknown;
    }
};

/**
 * @brief Helper to get the name of a JSON type.
 *
 */
struct GetJsonTypeName
{
    /**
     * @brief Return the name of the JSON type or empty if unknown.
     *
     * @param type Type to name.
     * @return const std::string& Name of type.
     */
    static const std::string &fromType(JsonType type);
};
} // namespace brayns
