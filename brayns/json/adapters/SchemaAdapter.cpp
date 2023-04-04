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

class ObjectSerializer
{
public:
    static void serialize(const brayns::JsonSchema &schema, brayns::JsonObject &object)
    {
        Helper::set(object, "properties", schema.properties);
        Helper::set(object, "required", _getRequired(schema.properties));
        Helper::set(object, "additionalProperties", false);
    }

private:
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
    if (!schema.enums.empty())
    {
        Helper::set(object, "enum", schema.enums);
        return;
    }
    if (!schema.properties.empty())
    {
        ObjectSerializer::serialize(schema, object);
        return;
    }
    if (JsonTypeInfo::isNumeric(schema.type))
    {
        Helper::set(object, "minimum", schema.minimum);
        Helper::set(object, "maximum", schema.maximum);
        return;
    }
    if (schema.type == JsonType::Array)
    {
        Helper::set(object, "items", schema.items);
        Helper::set(object, "minItems", schema.minItems);
        Helper::set(object, "maxItems", schema.maxItems);
        return;
    }
    if (schema.type == JsonType::Object)
    {
        Helper::set(object, "additionalProperties", schema.items);
        return;
    }
}

void JsonAdapter<JsonSchema>::deserialize(const JsonValue &json, JsonSchema &schema)
{
    (void)json;
    (void)schema;
    throw std::runtime_error("JSON schemas deserialization not supported");
}
} // namespace brayns
