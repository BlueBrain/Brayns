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

#include "JsonObjectInfo.h"

#include "JsonExtractor.h"

namespace
{
class JsonObjectHelper
{
public:
    static brayns::JsonSchema getSchemaWithOptions(const brayns::JsonObjectProperty &property, const void *message)
    {
        auto schema = property.getSchema(message);
        auto &options = property.options;
        brayns::JsonSchemaOptions::add(schema, options);
        return schema;
    }

    static void add(const brayns::JsonObjectProperty &property, brayns::JsonSchema &schema, const void *message)
    {
        auto &key = property.name;
        if (isRequired(property))
        {
            auto &required = schema.required;
            required.push_back(key);
        }
        auto &properties = schema.properties;
        properties[key] = getSchemaWithOptions(property, message);
    }

    static brayns::JsonValue extract(const brayns::JsonObjectProperty &property, const brayns::JsonObject &object)
    {
        auto &key = property.name;
        auto json = object.get(key);
        if (!json.isEmpty())
        {
            return json;
        }
        auto &options = property.options;
        auto &defaultValue = options.defaultValue;
        if (!defaultValue)
        {
            return json;
        }
        return *defaultValue;
    }

    static bool isRequired(const brayns::JsonObjectProperty &property)
    {
        auto &options = property.options;
        auto &required = options.required;
        return required.value_or(false);
    }

    static bool isReadOnly(const brayns::JsonObjectProperty &property)
    {
        auto &options = property.options;
        auto &readOnly = options.readOnly;
        return readOnly.value_or(false);
    }

    static bool isWriteOnly(const brayns::JsonObjectProperty &property)
    {
        auto &options = property.options;
        auto &writeOnly = options.writeOnly;
        return writeOnly.value_or(false);
    }
};
} // namespace

namespace brayns
{
JsonObjectInfo::JsonObjectInfo(std::string title)
    : _title(std::move(title))
{
}

JsonSchema JsonObjectInfo::getSchema(const void *message) const
{
    JsonSchema schema;
    schema.title = _title;
    schema.type = JsonType::Object;
    for (const auto &property : _properties)
    {
        JsonObjectHelper::add(property, schema, message);
    }
    return schema;
}

bool JsonObjectInfo::serialize(const void *message, JsonValue &json) const
{
    auto object = Poco::makeShared<JsonObject>();
    for (const auto &property : _properties)
    {
        if (JsonObjectHelper::isWriteOnly(property))
        {
            continue;
        }
        JsonValue child;
        if (property.serialize(message, child))
        {
            object->set(property.name, child);
        }
    }
    json = object;
    return true;
}

bool JsonObjectInfo::deserialize(const JsonValue &json, void *message) const
{
    auto object = JsonExtractor::extractObject(json);
    if (!object)
    {
        return false;
    }
    for (const auto &property : _properties)
    {
        if (JsonObjectHelper::isReadOnly(property))
        {
            continue;
        }
        auto child = JsonObjectHelper::extract(property, *object);
        property.deserialize(child, message);
    }
    return true;
}

void JsonObjectInfo::addProperty(JsonObjectProperty property)
{
    _properties.push_back(std::move(property));
}
} // namespace brayns
