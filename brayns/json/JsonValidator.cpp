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

#include "JsonValidator.h"

#include <sstream>

#include <spdlog/fmt/fmt.h>

#include <brayns/utils/string/StringJoiner.h>

namespace brayns
{
void JsonValidator::validate(const JsonValue &json, const JsonSchema &schema, JsonErrors &errors)
{
    if (json.isEmpty())
    {
        schema.holder.validate(schema.options.defaultValue, errors);
        return;
    }
    schema.holder.validate(json, errors);
}

bool JsonTypeChecker::check(JsonType type, JsonType required)
{
    if (type == required)
    {
        return true;
    }
    if (required == JsonType::Undefined)
    {
        return true;
    }
    if (required == JsonType::Number && type == JsonType::Integer)
    {
        return true;
    }
    return false;
}

bool JsonTypeChecker::check(JsonType type, JsonType required, JsonErrors &errors)
{
    if (check(type, required))
    {
        return true;
    }
    auto &got = EnumInfo::getName(type);
    auto &expected = EnumInfo::getName(required);
    errors.add("invalid type: expected {}, got {}", expected, got);
    return false;
}

bool JsonTypeChecker::check(const JsonValue &json, JsonType required, JsonErrors &errors)
{
    auto type = JsonTypeInfo::getType(json);
    return check(type, required, errors);
}

bool JsonRangeChecker::checkItemCount(size_t size, const JsonRange<size_t> &range, JsonErrors &errors)
{
    if (size < range.min)
    {
        errors.add("item count below minimum: {} < {}", size, range.min);
        return false;
    }
    if (size > range.max)
    {
        errors.add("item count above maximum: {} > {}", size, range.max);
        return false;
    }
    return true;
}

bool JsonPropertyChecker::checkRequired(const JsonObject &json, const ObjectSchema &schema, JsonErrors &errors)
{
    auto missing = false;
    for (const auto &key : schema.required)
    {
        if (json.has(key))
        {
            continue;
        }
        missing = true;
        errors.add("missing required property: '{}'", key);
    }
    return !missing;
}

bool JsonPropertyChecker::checkUnknown(const std::string &key, const ObjectSchema &schema, JsonErrors &errors)
{
    if (schema.properties.find(key) != schema.properties.end())
    {
        return true;
    }
    errors.add("unknown property: '{}'", key);
    return false;
}

void JsonSchemaValidator<ArraySchema>::validate(const JsonValue &json, const ArraySchema &schema, JsonErrors &errors)
{
    if (!JsonTypeChecker::check(json, JsonType::Array, errors))
    {
        return;
    }
    auto &array = *json.extract<JsonArray::Ptr>();
    JsonRangeChecker::checkItemCount(array.size(), schema.itemCount, errors);
    auto i = size_t(0);
    for (const auto &child : array)
    {
        errors.push(i);
        JsonValidator::validate(child, schema.items, errors);
        errors.pop();
        ++i;
    }
}

void JsonSchemaValidator<MapSchema>::validate(const JsonValue &json, const MapSchema &schema, JsonErrors &errors)
{
    if (!JsonTypeChecker::check(json, JsonType::Object, errors))
    {
        return;
    }
    auto &object = *json.extract<JsonObject::Ptr>();
    for (const auto &[key, value] : object)
    {
        errors.push(key);
        JsonValidator::validate(value, schema.items, errors);
        errors.pop();
    }
}

void JsonSchemaValidator<ObjectSchema>::validate(const JsonValue &json, const ObjectSchema &schema, JsonErrors &errors)
{
    if (!JsonTypeChecker::check(json, JsonType::Object, errors))
    {
        return;
    }
    auto &object = *json.extract<JsonObject::Ptr>();
    JsonPropertyChecker::checkRequired(object, schema, errors);
    for (const auto &[key, value] : object)
    {
        if (!JsonPropertyChecker::checkUnknown(key, schema, errors))
        {
            continue;
        }
        errors.push(key);
        auto &items = schema.properties.at(key);
        JsonValidator::validate(value, items, errors);
        errors.pop();
    }
}

void JsonSchemaValidator<EnumSchema>::validate(const JsonValue &json, const EnumSchema &schema, JsonErrors &errors)
{
    if (!JsonTypeChecker::check(json, JsonType::String, errors))
    {
        return;
    }
    auto &enums = schema.enums;
    auto &value = json.extract<std::string>();
    if (std::find(enums.begin(), enums.end(), value) != schema.enums.end())
    {
        return;
    }
    auto stream = std::ostringstream();
    if (!enums.empty())
    {
        stream << "'" << enums[0] << "'";
    }
    for (const auto &item : enums)
    {
        stream << ", '" << item << "'";
    }
    errors.add("invalid enum value: '{}' not in [{}]", value, stream.str());
}

void JsonSchemaValidator<OneOfSchema>::validate(const JsonValue &json, const OneOfSchema &schema, JsonErrors &errors)
{
    for (const auto &oneOf : schema.oneOf)
    {
        auto mismatch = JsonErrors();
        JsonValidator::validate(json, oneOf, errors);
        if (mismatch.isEmpty())
        {
            return;
        }
    }
    errors.add("invalid oneOf");
}
} // namespace brayns
