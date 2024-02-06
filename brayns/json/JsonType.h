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
#include <type_traits>
#include <unordered_map>

#include <spdlog/fmt/fmt.h>

#include <Poco/JSON/Array.h>
#include <Poco/JSON/JSONException.h>
#include <Poco/JSON/Object.h>

#include <brayns/utils/EnumInfo.h>

namespace brayns
{
/**
 * @brief JSON value among object, array, string, number, bool or null.
 *
 */
using JsonValue = Poco::Dynamic::Var;

/**
 * @brief Null JSON.
 *
 */
struct EmptyJson
{
};

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
 * @brief Exception thrown when the JSON parsing fails.
 *
 */
using JsonParsingError = Poco::JSON::JSONException;

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

/**
 * @brief Reflect JsonType keys.
 *
 */
template<>
struct EnumReflector<JsonType>
{
    static EnumMap<JsonType> reflect();
};

/**
 * @brief Helper class to get info about JSON type.
 *
 */
class JsonTypeInfo
{
public:
    /**
     * @brief Return the JSON type from a JSON value.
     *
     * @param json JSON to evaluate.
     * @return JsonType JSON type.
     * @throw std::invalid_argument Not a valid JSON type.
     */
    static JsonType getType(const JsonValue &json);

    /**
     * @brief Check if the given type is numeric (integer or number).
     *
     * @param type JSON type to check.
     * @return true Type is numeric.
     * @return false Type is not numeric.
     */
    static constexpr bool isNumeric(JsonType type)
    {
        return type == JsonType::Integer || type == JsonType::Number;
    }

    /**
     * @brief Map primitive JSON types to C++ type.
     *
     * @tparam T Type to map.
     * @return JsonType JSON type of T.
     */
    template<typename T>
    static constexpr JsonType getType()
    {
        if constexpr (std::is_same_v<T, JsonValue>)
        {
            return JsonType::Undefined;
        }
        else if constexpr (std::is_same_v<T, EmptyJson>)
        {
            return JsonType::Null;
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            return JsonType::Boolean;
        }
        else if constexpr (std::is_integral_v<T>)
        {
            return JsonType::Integer;
        }
        else if constexpr (std::is_arithmetic_v<T>)
        {
            return JsonType::Number;
        }
        else if constexpr (std::is_same_v<T, std::string>)
        {
            return JsonType::String;
        }
        else
        {
            static_assert(_alwaysFalse<T>, "Not a primitive type");
        }
    }

    /**
     * @brief Check if the given C++ type is numeric from a JSON perspective.
     *
     * @tparam T Type to check.
     * @return true T is numeric.
     * @return false T is not numeric.
     */
    template<typename T>
    static constexpr bool isNumeric()
    {
        auto type = getType<T>();
        return isNumeric(type);
    }

private:
    template<typename T>
    static constexpr auto _alwaysFalse = false;
};

/**
 * @brief Helper class to extract JSON elements.
 *
 */
class JsonExtractor
{
public:
    static const JsonArray &extractArray(const JsonValue &json);
    static const JsonObject &extractObject(const JsonValue &json);
};

/**
 * @brief Helper class to create JSON elements.
 *
 */
class JsonFactory
{
public:
    static JsonValue array();
    static JsonValue object();
    static JsonArray &emplaceArray(JsonValue &json);
    static JsonObject &emplaceObject(JsonValue &json);
};
} // namespace brayns

namespace fmt
{
template<>
struct formatter<brayns::JsonType> : fmt::formatter<std::string>
{
    auto format(const brayns::JsonType &type, fmt::format_context &context) const
    {
        auto &name = brayns::EnumInfo::getName(type);
        return fmt::formatter<std::string>::format(name, context);
    }
};
} // namespace fmt
