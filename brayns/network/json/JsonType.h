/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/common/utils/enumUtils.h>

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
template <typename T>
using StringMap = std::map<std::string, T>;

/**
 * @brief Shorcut for unordered_map<string, T> to avoid commas in macros.
 *
 * @tparam T Map value type.
 */
template <typename T>
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

struct JsonTypeHelper
{
    static bool check(JsonType required, JsonType type)
    {
        return required == type ||
               (required == JsonType::Number && type == JsonType::Integer);
    }

    static bool isNumber(JsonType type)
    {
        return type == JsonType::Integer || type == JsonType::Number;
    }

    static bool isPrimitive(JsonType type)
    {
        return type != JsonType::Unknown && type <= JsonType::String;
    }

    static bool isPrimitive(const JsonValue& json)
    {
        return json.isNumeric() || json.isString();
    }

    template <typename T>
    static constexpr bool isPrimitive()
    {
        return std::is_arithmetic<T>() || std::is_same<T, std::string>();
    }
};

class JsonTypeName
{
public:
    static const std::string& ofNull()
    {
        static const std::string name = "null";
        return name;
    }

    static const std::string& ofBoolean()
    {
        static const std::string name = "boolean";
        return name;
    }

    static const std::string& ofInteger()
    {
        static const std::string name = "integer";
        return name;
    }

    static const std::string& ofNumber()
    {
        static const std::string name = "number";
        return name;
    }

    static const std::string& ofString()
    {
        static const std::string name = "string";
        return name;
    }

    static const std::string& ofArray()
    {
        static const std::string name = "array";
        return name;
    }

    static const std::string& ofObject()
    {
        static const std::string name = "object";
        return name;
    }
};

struct GetJsonType
{
    static JsonType fromName(const std::string& name)
    {
        if (name == JsonTypeName::ofNull())
        {
            return JsonType::Null;
        }
        if (name == JsonTypeName::ofBoolean())
        {
            return JsonType::Boolean;
        }
        if (name == JsonTypeName::ofInteger())
        {
            return JsonType::Integer;
        }
        if (name == JsonTypeName::ofNumber())
        {
            return JsonType::Number;
        }
        if (name == JsonTypeName::ofString())
        {
            return JsonType::String;
        }
        if (name == JsonTypeName::ofArray())
        {
            return JsonType::Array;
        }
        if (name == JsonTypeName::ofObject())
        {
            return JsonType::Object;
        }
        return JsonType::Unknown;
    }

    static JsonType fromJson(const JsonValue& json)
    {
        if (json.isEmpty())
        {
            return JsonType::Null;
        }
        if (json.isBoolean())
        {
            return JsonType::Boolean;
        }
        if (json.isInteger())
        {
            return JsonType::Integer;
        }
        if (json.isNumeric())
        {
            return JsonType::Number;
        }
        if (json.isString())
        {
            return JsonType::String;
        }
        if (json.type() == typeid(JsonArray::Ptr))
        {
            return JsonType::Array;
        }
        if (json.type() == typeid(JsonObject::Ptr))
        {
            return JsonType::Object;
        }
        return JsonType::Unknown;
    }

    template <typename T>
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

struct GetJsonTypeName
{
    static const std::string& fromType(JsonType type)
    {
        static const std::string empty;
        switch (type)
        {
        case JsonType::Null:
            return JsonTypeName::ofNull();
        case JsonType::Boolean:
            return JsonTypeName::ofBoolean();
        case JsonType::Integer:
            return JsonTypeName::ofInteger();
        case JsonType::Number:
            return JsonTypeName::ofNumber();
        case JsonType::String:
            return JsonTypeName::ofString();
        case JsonType::Array:
            return JsonTypeName::ofArray();
        case JsonType::Object:
            return JsonTypeName::ofObject();
        default:
            return empty;
        }
    }
};
} // namespace brayns