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

#include "Schema.h"

#include "Arrays.h"
#include "Enums.h"
#include "Maps.h"
#include "Primitives.h"

namespace
{
using namespace brayns::experimental;

template<typename T>
void set(JsonObject &object, const std::string &key, const T &value)
{
    auto json = serializeToJson(value);
    object.set(key, json);
}

void serializeNumber(JsonObject &object, const JsonSchema &schema)
{
    if (schema.minimum)
    {
        set(object, "minimum", *schema.minimum);
    }

    if (schema.maximum)
    {
        set(object, "maximum", *schema.maximum);
    }
}

void serializeArray(JsonObject &object, const JsonSchema &schema)
{
    const auto &items = schema.items.at(0);

    if (items.type != JsonType::Undefined)
    {
        set(object, "items", items);
    }

    if (schema.minItems)
    {
        set(object, "minItems", *schema.minItems);
    }

    if (schema.maxItems)
    {
        set(object, "maxItems", *schema.maxItems);
    }
}

void serializeMap(JsonObject &object, const JsonSchema &schema)
{
    const auto &items = schema.items.at(0);

    if (items.type != JsonType::Undefined)
    {
        set(object, "additionalProperties", items);
    }
}

std::vector<std::string> extractRequiredProperties(const JsonSchema &schema)
{
    const auto &properties = schema.properties;

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

void serializeObject(JsonObject &object, const JsonSchema &schema)
{
    set(object, "additionalProperties", false);

    const auto &properties = schema.properties;

    if (properties.empty())
    {
        return;
    }

    set(object, "properties", properties);

    auto required = extractRequiredProperties(schema);

    if (!required.empty())
    {
        set(object, "required", required);
    }
}
}

namespace brayns::experimental
{
JsonSchema JsonReflector<JsonSchema>::getSchema()
{
    return JsonSchema{
        .type = JsonType::Object,
        .items = {JsonSchema()},
    };
}

JsonValue JsonReflector<JsonSchema>::serialize(const JsonSchema &schema)
{
    auto object = createJsonObject();

    if (!schema.description.empty())
    {
        set(*object, "description", schema.description);
    }

    if (!schema.required)
    {
        set(*object, "default", schema.defaultValue);
    }

    if (!schema.oneOf.empty())
    {
        set(*object, "oneOf", schema.oneOf);
        return object;
    }

    if (schema.type != JsonType::Undefined)
    {
        set(*object, "type", schema.type);
    }

    if (!schema.constant.empty())
    {
        set(*object, "const", schema.constant);
        return object;
    }

    if (isNumeric(schema.type))
    {
        serializeNumber(*object, schema);
        return object;
    }

    if (schema.type == JsonType::Array)
    {
        serializeArray(*object, schema);
        return object;
    }

    if (schema.type != JsonType::Object)
    {
        return object;
    }

    if (!schema.items.empty())
    {
        serializeMap(*object, schema);
        return object;
    }

    serializeObject(*object, schema);

    return object;
}
}
