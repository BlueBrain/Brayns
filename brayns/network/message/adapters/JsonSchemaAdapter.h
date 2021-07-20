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

#include <brayns/network/message/Json.h>

#include "ArrayAdapter.h"
#include "EnumAdapter.h"
#include "PrimitiveAdapter.h"

namespace brayns
{
class JsonSchemaSerializer
{
public:
    static void serialize(const JsonSchema& schema, JsonObject& object)
    {
        setIfNotEmpty(object, "oneOf", schema.oneOf);
        setIfNotEmpty(object, "title", schema.title);
        setIfNotEmpty(object, "description", schema.description);
        setType(object, schema.type);
        setIfNotNull(object, "readOnly", schema.readOnly);
        setIfNotNull(object, "writeOnly", schema.writeOnly);
        setIfNotEmpty(object, "default", schema.defaultValue);
        setIfNotNull(object, "minimum", schema.minimum);
        setIfNotNull(object, "maximum", schema.maximum);
        setIfNotEmpty(object, "enum", schema.enums);
        setIfNotEmpty(object, "properties", schema.properties);
        setIfNotEmpty(object, "required", schema.required);
        setAdditionalProperties(object, "additionalProperties", schema);
        setAsTuple(object, "items", schema.items);
        setIfNotNull(object, "minItems", schema.minItems);
        setIfNotNull(object, "maxItems", schema.maxItems);
    }

private:
    static void setType(JsonObject& object, JsonType type)
    {
        if (type == JsonType::Null)
        {
            return;
        }
        object.set("type", GetJsonTypeName::fromType(type));
    }

    static void setIfNotEmpty(JsonObject& object, const std::string& key,
                              const JsonValue& value)
    {
        if (value.isEmpty())
        {
            return;
        }
        set(object, key, value);
    }

    static void setAdditionalProperties(JsonObject& object,
                                        const std::string& key,
                                        const JsonSchema& value)
    {
        if (value.type != JsonType::Object)
        {
            return;
        }
        auto& additionalProperties = value.additionalProperties;
        if (additionalProperties.empty())
        {
            set(object, key, false);
            return;
        }
        if (additionalProperties.size() == 1)
        {
            set(object, key, additionalProperties[0]);
            return;
        }
        set(object, key, additionalProperties);
    }

    template <typename T>
    static void set(JsonObject& object, const std::string& key, const T& value)
    {
        object.set(key, Json::serialize(value));
    }

    template <typename T>
    static void setIfNotNull(JsonObject& object, const std::string& key,
                             const T& value)
    {
        if (!value)
        {
            return;
        }
        set(object, key, value);
    }

    template <typename T>
    static void setIfNotEmpty(JsonObject& object, const std::string& key,
                              const T& value)
    {
        if (value.empty())
        {
            return;
        }
        set(object, key, value);
    }

    template <typename T>
    static void setFirstIfNotEmpty(JsonObject& object, const std::string& key,
                                   const T& value)
    {
        if (value.empty())
        {
            return;
        }
        set(object, key, value[0]);
    }

    template <typename T>
    static void setAsTuple(JsonObject& object, const std::string& key,
                           const T& value)
    {
        if (value.empty())
        {
            return;
        }
        if (value.size() == 1)
        {
            set(object, key, value[0]);
            return;
        }
        set(object, key, value);
    }
};

class JsonSchemaDeserializer
{
public:
    static void deserialize(const JsonObject& object, JsonSchema& schema)
    {
        get(object, "oneOf", schema.oneOf);
        get(object, "title", schema.title);
        get(object, "description", schema.description);
        get(object, "readOnly", schema.readOnly);
        get(object, "writeOnly", schema.writeOnly);
        get(object, "default", schema.defaultValue);
        get(object, "type", schema.type);
        get(object, "minimum", schema.minimum);
        get(object, "maximum", schema.maximum);
        get(object, "enum", schema.enums);
        get(object, "properties", schema.properties);
        get(object, "required", schema.required);
        get(object, "additionalProperties", schema.additionalProperties);
        get(object, "items", schema.items);
        get(object, "minItems", schema.minItems);
        get(object, "maxItems", schema.maxItems);
    }

private:
    template <typename T>
    static void get(const JsonObject& object, const std::string& key, T& value)
    {
        auto json = object.get(key);
        Json::deserialize(json, value);
    }
};

template <>
struct JsonAdapter<JsonSchema>
{
    static JsonSchema getSchema(const JsonSchema& schema) { return schema; }

    static bool serialize(const JsonSchema& value, JsonValue& json)
    {
        auto object = Poco::makeShared<JsonObject>();
        JsonSchemaSerializer::serialize(value, *object);
        json = object;
        return true;
    }

    static bool deserialize(const JsonValue& json, JsonSchema& value)
    {
        auto object = JsonHelper::extractObject(json);
        if (!object)
        {
            return false;
        }
        JsonSchemaDeserializer::deserialize(*object, value);
        return true;
    }
};
} // namespace brayns