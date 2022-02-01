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

#include "JsonType.h"

namespace brayns
{
bool JsonTypeHelper::check(JsonType required, JsonType type)
{
    return required == type || (required == JsonType::Number && type == JsonType::Integer);
}

bool JsonTypeHelper::isNumeric(JsonType type)
{
    return type == JsonType::Integer || type == JsonType::Number;
}

bool JsonTypeHelper::isPrimitive(JsonType type)
{
    return type != JsonType::Unknown && type <= JsonType::String;
}

bool JsonTypeHelper::isPrimitive(const JsonValue &json)
{
    return json.isNumeric() || json.isString();
}

const std::string &JsonTypeName::ofNull()
{
    static const std::string name = "null";
    return name;
}

const std::string &JsonTypeName::ofBoolean()
{
    static const std::string name = "boolean";
    return name;
}

const std::string &JsonTypeName::ofInteger()
{
    static const std::string name = "integer";
    return name;
}

const std::string &JsonTypeName::ofNumber()
{
    static const std::string name = "number";
    return name;
}

const std::string &JsonTypeName::ofString()
{
    static const std::string name = "string";
    return name;
}

const std::string &JsonTypeName::ofArray()
{
    static const std::string name = "array";
    return name;
}

const std::string &JsonTypeName::ofObject()
{
    static const std::string name = "object";
    return name;
}

JsonType GetJsonType::fromName(const std::string &name)
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

JsonType GetJsonType::fromJson(const JsonValue &json)
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

const std::string &GetJsonTypeName::fromType(JsonType type)
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
} // namespace brayns
