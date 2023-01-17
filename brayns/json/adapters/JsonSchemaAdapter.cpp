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

#include "JsonSchemaAdapter.h"

#include <brayns/json/Json.h>

#include "ArrayAdapter.h"
#include "EnumAdapter.h"
#include "MapAdapter.h"
#include "PrimitiveAdapter.h"
#include "PtrAdapter.h"

namespace
{
class JsonSchemaSerializer
{
public:
    static void serialize(const brayns::JsonSchema &schema, brayns::JsonObject &object)
    {
        setIfNotEmpty(object, "oneOf", schema.oneOf);
        setIfNotEmpty(object, "title", schema.title);
        setIfNotEmpty(object, "description", schema.description);
        setType(object, "type", schema.type);
        setIfNotNull(object, "readOnly", schema.readOnly);
        setIfNotNull(object, "writeOnly", schema.writeOnly);
        setIfNotEmpty(object, "default", schema.defaultValue);
        setIfNotNull(object, "minimum", schema.minimum);
        setIfNotNull(object, "maximum", schema.maximum);
        setIfNotEmpty(object, "enum", schema.enums);
        setIfNotEmpty(object, "properties", schema.properties);
        setIfNotEmpty(object, "required", schema.required);
        setAdditionalProperties(object, "additionalProperties", schema);
        setItems(object, "items", schema.items);
        setIfNotNull(object, "minItems", schema.minItems);
        setIfNotNull(object, "maxItems", schema.maxItems);
    }

private:
    static void setType(brayns::JsonObject &object, const std::string &key, brayns::JsonType type)
    {
        if (type == brayns::JsonType::Undefined)
        {
            return;
        }
        set(object, key, type);
    }

    static void setIfNotEmpty(brayns::JsonObject &object, const std::string &key, const brayns::JsonValue &value)
    {
        if (value.isEmpty())
        {
            return;
        }
        set(object, key, value);
    }

    static void
        setAdditionalProperties(brayns::JsonObject &object, const std::string &key, const brayns::JsonSchema &value)
    {
        if (value.type != brayns::JsonType::Object)
        {
            return;
        }
        auto &additionalProperties = value.additionalProperties;
        if (additionalProperties.empty())
        {
            set(object, key, false);
            return;
        }
        auto &schema = additionalProperties[0];
        if (brayns::JsonSchemaHelper::isWildcard(schema))
        {
            return;
        }
        set(object, key, schema);
    }

    static void
        setItems(brayns::JsonObject &object, const std::string &key, const std::vector<brayns::JsonSchema> &value)
    {
        if (value.empty())
        {
            return;
        }
        set(object, key, value[0]);
    }

    template<typename T>
    static void set(brayns::JsonObject &object, const std::string &key, const T &value)
    {
        object.set(key, brayns::Json::serialize(value));
    }

    template<typename T>
    static void setIfNotNull(brayns::JsonObject &object, const std::string &key, const T &value)
    {
        if (!value)
        {
            return;
        }
        set(object, key, value);
    }

    template<typename T>
    static void setIfNotEmpty(brayns::JsonObject &object, const std::string &key, const T &value)
    {
        if (value.empty())
        {
            return;
        }
        set(object, key, value);
    }
};

class JsonSchemaDeserializer
{
public:
    static void deserialize(const brayns::JsonObject &object, brayns::JsonSchema &schema)
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
    template<typename T>
    static void get(const brayns::JsonObject &object, const std::string &key, T &value)
    {
        auto json = object.get(key);
        brayns::Json::deserialize(json, value);
    }
};
} // namespace

namespace brayns
{
JsonSchema JsonAdapter<JsonSchema>::getSchema()
{
    JsonSchema schema;
    schema.type = JsonType::Object;
    JsonSchemaHelper::allowAnyAdditionalProperty(schema);
    return schema;
}

void JsonAdapter<JsonSchema>::serialize(const JsonSchema &value, JsonValue &json)
{
    auto object = Poco::makeShared<JsonObject>();
    JsonSchemaSerializer::serialize(value, *object);
    json = object;
}

void JsonAdapter<JsonSchema>::deserialize(const JsonValue &json, JsonSchema &value)
{
    auto object = JsonExtractor::extractObject(json);
    if (!object)
    {
        throw std::runtime_error("Not a JSON object");
    }
    JsonSchemaDeserializer::deserialize(*object, value);
}
} // namespace brayns
