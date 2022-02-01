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

#pragma once

#include <functional>
#include <string>
#include <vector>

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
 * @brief Json property metadata.
 *
 */
struct JsonObjectProperty
{
    std::string name;
    JsonOptions options;
    std::function<JsonSchema(const void *)> getSchema;
    std::function<bool(const void *, JsonValue &)> serialize;
    std::function<bool(const JsonValue &, void *)> deserialize;

    JsonSchema getSchemaWithOptions(const void *message) const;
    void add(JsonSchema &schema, const void *message) const;
    JsonValue extract(const JsonObject &object) const;
    bool isRequired() const;
    bool isReadOnly() const;
    bool isWriteOnly() const;
};

/**
 * @brief Json object metadata.
 *
 */
class JsonObjectInfo
{
public:
    JsonObjectInfo() = default;
    JsonObjectInfo(std::string title);

    JsonSchema getSchema(const void *message) const;
    bool serialize(const void *message, JsonValue &json) const;
    bool deserialize(const JsonValue &json, void *message) const;
    void addProperty(JsonObjectProperty property);

private:
    std::string _title;
    std::vector<JsonObjectProperty> _properties;
};
} // namespace brayns
