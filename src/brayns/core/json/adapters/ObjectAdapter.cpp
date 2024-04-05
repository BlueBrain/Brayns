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

#include "ObjectAdapter.h"

namespace brayns
{
JsonSchema JsonObjectHandler::getSchema(const JsonObjectInfo &object)
{
    auto schema = JsonSchema();
    schema.title = object.title;
    schema.type = JsonType::Object;
    for (const auto &property : object.properties)
    {
        schema.properties[property.name] = property.schema;
    }
    return schema;
}

void JsonObjectHandler::serialize(const JsonObjectInfo &object, const void *value, JsonValue &json)
{
    auto &result = JsonFactory::emplaceObject(json);
    for (const auto &property : object.properties)
    {
        if (property.schema.writeOnly)
        {
            continue;
        }
        auto child = JsonValue();
        property.serialize(value, child);
        if (child.isEmpty() && !property.schema.required)
        {
            continue;
        }
        result.set(property.name, child);
    }
}

void JsonObjectHandler::deserialize(const JsonObjectInfo &object, const JsonValue &json, void *value)
{
    auto &result = JsonExtractor::extractObject(json);
    for (const auto &property : object.properties)
    {
        if (property.schema.readOnly)
        {
            continue;
        }
        auto child = result.get(property.name);
        if (child.isEmpty())
        {
            child = property.schema.defaultValue;
        }
        if (child.isEmpty() && !property.schema.required)
        {
            continue;
        }
        property.deserialize(child, value);
    }
}

JsonPropertyBuilder::JsonPropertyBuilder(JsonProperty &property):
    _property(property)
{
}

JsonPropertyBuilder JsonPropertyBuilder::description(std::string value)
{
    _property.schema.description = std::move(value);
    return *this;
}

JsonPropertyBuilder JsonPropertyBuilder::required(bool value)
{
    _property.schema.required = value;
    return *this;
}

JsonPropertyBuilder JsonPropertyBuilder::readOnly(bool value)
{
    _property.schema.readOnly = value;
    return *this;
}

JsonPropertyBuilder JsonPropertyBuilder::writeOnly(bool value)
{
    _property.schema.writeOnly = value;
    return *this;
}

JsonPropertyBuilder JsonPropertyBuilder::minimum(double value)
{
    _property.schema.minimum = value;
    return *this;
}

JsonPropertyBuilder JsonPropertyBuilder::maximum(double value)
{
    _property.schema.maximum = value;
    return *this;
}

JsonPropertyBuilder JsonPropertyBuilder::minItems(size_t value)
{
    _property.schema.minItems = value;
    return *this;
}

JsonPropertyBuilder JsonPropertyBuilder::maxItems(size_t value)
{
    _property.schema.maxItems = value;
    return *this;
}
} // namespace brayns
