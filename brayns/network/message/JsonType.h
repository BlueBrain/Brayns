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

#include <string>
#include <type_traits>

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

enum class JsonType
{
    Null,
    Boolean,
    Integer,
    Number,
    String,
    Array,
    Object
};

struct JsonTypeInfo
{
    static bool isNumber(JsonType type)
    {
        return type == JsonType::Integer || type == JsonType::Number;
    }

    static bool isPrimitive(JsonType type) { return type <= JsonType::String; }

    static bool isSame(JsonType left, JsonType right)
    {
        return left == right || isNumber(left) && isNumber(right);
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

struct JsonTypeNameInfo
{
    static bool isNumber(const std::string& name)
    {
        return name == JsonTypeName::ofInteger() ||
               name == JsonTypeName::ofNumber();
    }
};

struct GetJsonType
{
    static JsonType fromName(const std::string& name)
    {
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
        return JsonType::Null;
    }

    template <typename T>
    static constexpr JsonType fromPrimitive()
    {
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
        return JsonType::Null;
    }
};

struct GetJsonTypeName
{
    static const std::string& fromType(JsonType type)
    {
        switch (type)
        {
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
        }
        return JsonTypeName::ofNull();
    }

    static const std::string& fromJson(const JsonValue& json)
    {
        if (json.isBoolean())
        {
            return JsonTypeName::ofBoolean();
        }
        if (json.isInteger())
        {
            return JsonTypeName::ofInteger();
        }
        if (json.isNumeric())
        {
            return JsonTypeName::ofNumber();
        }
        if (json.isString())
        {
            return JsonTypeName::ofString();
        }
        if (json.type() == typeid(JsonArray::Ptr))
        {
            return JsonTypeName::ofArray();
        }
        if (json.type() == typeid(JsonObject::Ptr))
        {
            return JsonTypeName::ofObject();
        }
        return JsonTypeName::ofNull();
    }

    template<typename T>
    static const std::string& fromPrimitive()
    {
        return fromType(GetJsonType::fromPrimitive<T>());
    }
};
} // namespace brayns