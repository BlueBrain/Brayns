/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

namespace brayns
{
JsonSchema JsonObjectProperty::getSchemaWithOptions(const void *message) const
{
    auto schema = getSchema(message);
    JsonSchemaOptions::add(schema, options);
    return schema;
}

void JsonObjectProperty::add(JsonSchema &schema, const void *message) const
{
    if (isRequired())
    {
        auto &required = schema.required;
        required.push_back(name);
    }
    schema.properties[name] = getSchemaWithOptions(message);
}

JsonValue JsonObjectProperty::extract(const JsonObject &object) const
{
    auto json = object.get(name);
    if (!json.isEmpty())
    {
        return json;
    }
    auto &defaultValue = options.defaultValue;
    if (!defaultValue)
    {
        return json;
    }
    return *defaultValue;
}

bool JsonObjectProperty::isRequired() const
{
    auto &required = options.required;
    return required.value_or(false);
}

bool JsonObjectProperty::isReadOnly() const
{
    auto &readOnly = options.readOnly;
    return readOnly.value_or(false);
}

bool JsonObjectProperty::isWriteOnly() const
{
    auto &writeOnly = options.writeOnly;
    return writeOnly.value_or(false);
}

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
        property.add(schema, message);
    }
    return schema;
}

bool JsonObjectInfo::serialize(const void *message, JsonValue &json) const
{
    auto object = Poco::makeShared<JsonObject>();
    for (const auto &property : _properties)
    {
        if (property.isWriteOnly())
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
        if (property.isReadOnly())
        {
            continue;
        }
        auto child = property.extract(*object);
        property.deserialize(child, message);
    }
    return true;
}

void JsonObjectInfo::addProperty(JsonObjectProperty property)
{
    _properties.push_back(std::move(property));
}
} // namespace brayns
