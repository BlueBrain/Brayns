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
    static brayns::JsonSchema getSchemaWithOptions(const brayns::JsonObjectProperty &property)
    {
        auto schema = property.getSchema();
        auto &options = property.options;
        brayns::JsonSchemaOptions::add(schema, options);
        return schema;
    }

    static void add(const brayns::JsonObjectProperty &property, brayns::JsonSchema &schema)
    {
        auto &key = property.name;
        if (isRequired(property))
        {
            auto &required = schema.required;
            required.push_back(key);
        }
        auto &properties = schema.properties;
        properties[key] = getSchemaWithOptions(property);
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
        return options.defaultValue;
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

    static bool canBeOmitted(const brayns::JsonObjectProperty &property, const brayns::JsonValue &value)
    {
        if (!value.isEmpty())
        {
            return false;
        }
        if (isRequired(property))
        {
            return false;
        }
        return true;
    }
};
} // namespace

namespace brayns
{
JsonObjectInfo::JsonObjectInfo(std::string title)
    : _title(std::move(title))
{
}

JsonSchema JsonObjectInfo::getSchema() const
{
    JsonSchema schema;
    schema.title = _title;
    schema.type = JsonType::Object;
    for (const auto &property : _properties)
    {
        JsonObjectHelper::add(property, schema);
    }
    return schema;
}

void JsonObjectInfo::serialize(const void *message, JsonValue &json) const
{
    auto object = Poco::makeShared<JsonObject>();
    for (const auto &property : _properties)
    {
        if (JsonObjectHelper::isWriteOnly(property))
        {
            continue;
        }
        JsonValue item;
        property.serialize(message, item);
        if (JsonObjectHelper::canBeOmitted(property, item))
        {
            continue;
        }
        object->set(property.name, item);
    }
    json = object;
}

void JsonObjectInfo::deserialize(const JsonValue &json, void *message) const
{
    auto object = JsonExtractor::extractObject(json);
    if (!object)
    {
        throw std::runtime_error("Not a JSON object");
    }
    for (const auto &property : _properties)
    {
        if (JsonObjectHelper::isReadOnly(property))
        {
            continue;
        }
        auto item = JsonObjectHelper::extract(property, *object);
        if (JsonObjectHelper::canBeOmitted(property, item))
        {
            continue;
        }
        property.deserialize(item, message);
    }
}

void JsonObjectInfo::addProperty(JsonObjectProperty property)
{
    _properties.push_back(std::move(property));
}
} // namespace brayns
