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

#include "JsonSchema.h"

#include <algorithm>

namespace brayns
{
bool JsonSchemaHelper::isWildcard(const JsonSchema &schema)
{
    return schema.type == JsonType::Undefined && schema.oneOf.empty();
}

bool JsonSchemaHelper::isNull(const JsonSchema &schema)
{
    return schema.type == JsonType::Null;
}

bool JsonSchemaHelper::isOneOf(const JsonSchema &schema)
{
    return !schema.oneOf.empty();
}

bool JsonSchemaHelper::isNumeric(const JsonSchema &schema)
{
    return JsonTypeHelper::isNumeric(schema.type);
}

bool JsonSchemaHelper::isEnum(const JsonSchema &schema)
{
    return !schema.enums.empty();
}

bool JsonSchemaHelper::isObject(const JsonSchema &schema)
{
    return schema.type == JsonType::Object;
}

bool JsonSchemaHelper::isArray(const JsonSchema &schema)
{
    return schema.type == JsonType::Array;
}

bool JsonSchemaHelper::hasProperty(const JsonSchema &schema, const std::string &key)
{
    auto &properties = schema.properties;
    return properties.find(key) != properties.end();
}

bool JsonSchemaHelper::isRequired(const JsonSchema &schema, const std::string &key)
{
    auto &required = schema.required;
    auto first = required.begin();
    auto last = required.end();
    return std::find(first, last, key) != last;
}

bool JsonSchemaHelper::checkType(const JsonSchema &schema, JsonType type)
{
    return JsonTypeHelper::check(schema.type, type);
}

void JsonSchemaHelper::allowAnyAdditionalProperty(JsonSchema &schema)
{
    schema.additionalProperties = {{}};
}

JsonSchema JsonSchemaHelper::getWildcardSchema()
{
    return {};
}

JsonSchema JsonSchemaHelper::getNullSchema()
{
    JsonSchema schema;
    schema.type = JsonType::Null;
    return schema;
}
} // namespace brayns
