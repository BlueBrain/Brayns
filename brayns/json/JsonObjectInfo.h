/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <brayns/json/adapters/ArrayAdapter.h>
#include <brayns/json/adapters/EnumAdapter.h>
#include <brayns/json/adapters/GlmAdapter.h>
#include <brayns/json/adapters/JsonSchemaAdapter.h>
#include <brayns/json/adapters/MapAdapter.h>
#include <brayns/json/adapters/PrimitiveAdapter.h>
#include <brayns/json/adapters/PtrAdapter.h>

#include <brayns/json/JsonOptions.h>
#include <brayns/json/JsonType.h>

namespace brayns
{
/**
 * @brief Json object property metadata.
 *
 */
struct JsonObjectProperty
{
    std::string name;
    JsonOptions options;
    std::function<JsonSchema(const void*)> getSchema;
    std::function<bool(const void*, JsonValue&)> serialize;
    std::function<bool(const JsonValue&, void*)> deserialize;

    JsonSchema getSchemaWithOptions(const void* message) const
    {
        auto schema = getSchema(message);
        JsonSchemaOptions::add(schema, options);
        return schema;
    }

    void add(JsonSchema& schema, const void* message) const
    {
        if (isRequired())
        {
            auto& required = schema.required;
            required.push_back(name);
        }
        schema.properties[name] = getSchemaWithOptions(message);
    }

    JsonValue extract(const JsonObject& object) const
    {
        auto json = object.get(name);
        if (!json.isEmpty())
        {
            return json;
        }
        auto& defaultValue = options.defaultValue;
        if (!defaultValue)
        {
            return json;
        }
        return *defaultValue;
    }

    bool isRequired() const
    {
        auto& required = options.required;
        return required.value_or(false);
    }

    bool isReadOnly() const
    {
        auto& readOnly = options.readOnly;
        return readOnly.value_or(false);
    }

    bool isWriteOnly() const
    {
        auto& writeOnly = options.writeOnly;
        return writeOnly.value_or(false);
    }
};

/**
 * @brief Json object metadata.
 *
 */
class JsonObjectInfo
{
public:
    JsonObjectInfo() = default;

    JsonObjectInfo(std::string title)
        : _title(std::move(title))
    {
    }

    JsonSchema getSchema(const void* message) const
    {
        JsonSchema schema;
        schema.title = _title;
        schema.type = JsonType::Object;
        for (const auto& property : _properties)
        {
            property.add(schema, message);
        }
        return schema;
    }

    bool serialize(const void* message, JsonValue& json) const
    {
        auto object = Poco::makeShared<JsonObject>();
        for (const auto& property : _properties)
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

    bool deserialize(const JsonValue& json, void* message) const
    {
        auto object = JsonExtractor::extractObject(json);
        if (!object)
        {
            return false;
        }
        for (const auto& property : _properties)
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

    void addProperty(JsonObjectProperty property)
    {
        _properties.push_back(std::move(property));
    }

private:
    std::string _title;
    std::vector<JsonObjectProperty> _properties;
};
} // namespace brayns
