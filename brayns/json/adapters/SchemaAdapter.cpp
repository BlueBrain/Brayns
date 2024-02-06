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

#include "SchemaAdapter.h"

#include "ArrayAdapter.h"
#include "EnumAdapter.h"
#include "MapAdapter.h"
#include "PrimitiveAdapter.h"

namespace
{
class Helper
{
public:
    template<typename T>
    static void set(brayns::JsonObject &object, const std::string &key, const T &value)
    {
        auto json = brayns::JsonValue();
        brayns::JsonAdapter<T>::serialize(value, json);
        object.set(key, json);
    }
};

class OptionSerializer
{
public:
    static void serialize(const brayns::JsonSchema &schema, brayns::JsonObject &object)
    {
        if (!schema.title.empty())
        {
            Helper::set(object, "title", schema.title);
        }
        if (!schema.description.empty())
        {
            Helper::set(object, "description", schema.description);
        }
        if (schema.readOnly)
        {
            Helper::set(object, "readOnly", true);
        }
        if (schema.writeOnly)
        {
            Helper::set(object, "writeOnly", true);
        }
        if (!schema.defaultValue.isEmpty())
        {
            Helper::set(object, "default", schema.defaultValue);
        }
    }
};

class NumberSerializer
{
public:
    static void serialize(const brayns::JsonSchema &schema, brayns::JsonObject &object)
    {
        if (schema.minimum)
        {
            Helper::set(object, "minimum", *schema.minimum);
        }
        if (schema.maximum)
        {
            Helper::set(object, "maximum", *schema.maximum);
        }
    }
};

class ArraySerializer
{
public:
    static void serialize(const brayns::JsonSchema &schema, brayns::JsonObject &object)
    {
        if (schema.items.empty())
        {
            throw std::invalid_argument("Invalid array schema without items");
        }
        auto &items = schema.items[0];
        if (items.type != brayns::JsonType::Undefined)
        {
            Helper::set(object, "items", items);
        }
        if (schema.minItems)
        {
            Helper::set(object, "minItems", *schema.minItems);
        }
        if (schema.maxItems)
        {
            Helper::set(object, "maxItems", *schema.maxItems);
        }
    }
};

class ObjectSerializer
{
public:
    static void serialize(const brayns::JsonSchema &schema, brayns::JsonObject &object)
    {
        if (!schema.items.empty())
        {
            _serializeMap(schema.items[0], object);
            return;
        }
        _serializeObject(schema.properties, object);
    }

private:
    static void _serializeMap(const brayns::JsonSchema &items, brayns::JsonObject &object)
    {
        if (items.type != brayns::JsonType::Undefined)
        {
            Helper::set(object, "additionalProperties", items);
        }
    }

    static void _serializeObject(const std::map<std::string, brayns::JsonSchema> &properties, brayns::JsonObject &object)
    {
        Helper::set(object, "additionalProperties", false);
        if (properties.empty())
        {
            return;
        }
        Helper::set(object, "properties", properties);
        auto required = _getRequired(properties);
        if (!required.empty())
        {
            Helper::set(object, "required", _getRequired(properties));
        }
    }

    static std::vector<std::string> _getRequired(const std::map<std::string, brayns::JsonSchema> &properties)
    {
        auto required = std::vector<std::string>();
        for (const auto &[key, value] : properties)
        {
            if (value.required)
            {
                required.push_back(key);
            }
        }
        return required;
    }
};
} // namespace

namespace brayns
{
JsonSchema JsonAdapter<JsonSchema>::getSchema()
{
    auto schema = JsonSchema();
    schema.type = JsonType::Object;
    schema.title = "JsonSchema";
    schema.items = {JsonSchema()};
    return schema;
}

void JsonAdapter<JsonSchema>::serialize(const JsonSchema &schema, JsonValue &json)
{
    auto &object = JsonFactory::emplaceObject(json);
    OptionSerializer::serialize(schema, object);
    if (!schema.oneOf.empty())
    {
        Helper::set(object, "oneOf", schema.oneOf);
        return;
    }
    if (schema.type != JsonType::Undefined)
    {
        Helper::set(object, "type", schema.type);
    }
    if (JsonTypeInfo::isNumeric(schema.type))
    {
        NumberSerializer::serialize(schema, object);
        return;
    }
    if (!schema.enums.empty())
    {
        Helper::set(object, "enum", schema.enums);
        return;
    }
    if (schema.type == JsonType::Array)
    {
        ArraySerializer::serialize(schema, object);
        return;
    }
    if (schema.type == JsonType::Object)
    {
        ObjectSerializer::serialize(schema, object);
        return;
    }
}

void JsonAdapter<JsonSchema>::deserialize(const JsonValue &json, JsonSchema &schema)
{
    (void)json;
    (void)schema;
    throw std::runtime_error("JSON schema deserialization not supported");
}
} // namespace brayns
