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

#include "JsonSchema.h"

using namespace brayns;

namespace
{
class JsonHelper
{
public:
    template <typename T>
    static void setIfNotEmpty(JsonObject& object, const std::string& key,
                              const T& value)
    {
        if (value.empty())
        {
            return;
        }
        object.set(key, value);
    }

    template <typename T>
    static void setIfNotNull(JsonObject& object, const std::string& key,
                             const T& value)
    {
        if (!value)
        {
            return;
        }
        object.set(key, *value);
    }
};

class JsonSchemaSerializer
{
public:
    static JsonObject::Ptr serialize(const JsonSchema& schema)
    {
        auto object = Poco::makeShared<JsonObject>();
        _addInfo(schema, *object);
        _addProperties(schema, *object);
        _addItems(schema, *object);
        return object;
    }

private:
    static void _addInfo(const JsonSchema& schema, JsonObject& object)
    {
        JsonHelper::setIfNotEmpty(object, "name", schema.name);
        JsonHelper::setIfNotEmpty(object, "description", schema.description);
        JsonHelper::setIfNotEmpty(object, "type", schema.type);
        JsonHelper::setIfNotNull(object, "minimum", schema.minimum);
        JsonHelper::setIfNotNull(object, "maximum", schema.maximum);
    }

    static void _addProperties(const JsonSchema& schema, JsonObject& object)
    {
        if (schema.properties.empty())
        {
            return;
        }
        auto properties = Poco::makeShared<JsonObject>();
        for (const auto& pair : schema.properties)
        {
            properties->set(pair.first, Json::serialize(pair.second));
        }
        object.set("properties", properties);
        object.set("additionalProperties", schema.additionalProperties);
    }

    static void _addItems(const JsonSchema& schema, JsonObject& object)
    {
        if (schema.items.empty())
        {
            return;
        }
        object.set("items", Json::serialize(schema.items[0]));
        JsonHelper::setIfNotNull(object, "minItems", schema.minItems);
        JsonHelper::setIfNotNull(object, "maxItems", schema.maxItems);
    }
};

class JsonSchemaDeserializer
{
public:
    static void deserialize(const JsonObject& object, JsonSchema& schema)
    {
        _addInfo(object, schema);
        _addProperties(object, schema);
        _addItems(object, schema);
    }

private:
    static void _addInfo(const JsonObject& object, JsonSchema& schema)
    {
        Json::deserialize(object.get("name"), schema.name);
        Json::deserialize(object.get("description"), schema.description);
        Json::deserialize(object.get("type"), schema.type);
        Json::deserialize(object.get("minimum"), schema.minimum);
        Json::deserialize(object.get("maximum"), schema.maximum);
    }

    static void _addProperties(const JsonObject& object, JsonSchema& schema)
    {
        auto properties = object.getObject("properties");
        if (!properties)
        {
            _loadProperties(*properties, schema);
        }
        Json::deserialize(object.get("additionalProperties"),
                          schema.additionalProperties);
    }

    static void _loadProperties(const JsonObject& object, JsonSchema& schema)
    {
        schema.properties.clear();
        for (const auto& pair : object)
        {
            schema.properties.emplace_back();
            auto& property = schema.properties.back();
            property.first = pair.first;
            Json::deserialize(pair.second, property.second);
        }
    }

    static void _addItems(const JsonObject& object, JsonSchema& schema)
    {
        auto items = object.get("items");
        if (!items.isEmpty())
        {
            schema.items = {Json::deserialize<JsonSchema>(items)};
        }
        Json::deserialize(object.get("minItems"), schema.minItems);
        Json::deserialize(object.get("maxItems"), schema.maxItems);
    }
};
} // namespace

namespace brayns
{
bool JsonSerializer<JsonSchema>::serialize(const JsonSchema& value,
                                           JsonValue& json)
{
    json = JsonSchemaSerializer::serialize(value);
    return true;
}

bool JsonSerializer<JsonSchema>::deserialize(const JsonValue& json,
                                             JsonSchema& value)
{
    auto object = JsonExtractor::extractObject(json);
    if (!object)
    {
        return false;
    }
    JsonSchemaDeserializer::deserialize(*object, value);
    return true;
}
} // namespace brayns