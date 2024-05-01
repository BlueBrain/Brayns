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

#include "JsonValidator.h"

#include <ranges>
#include <stdexcept>
#include <utility>

#include <fmt/format.h>

namespace
{
using namespace brayns::experimental;

class ErrorContext
{
public:
    void push(JsonPathItem item)
    {
        _path.push_back(std::move(item));
    }

    void pop()
    {
        _path.pop_back();
    }

    void add(JsonError error)
    {
        _errors.push_back({_path, std::move(error)});
    }

    std::vector<JsonSchemaError> build()
    {
        return std::exchange(_errors, {});
    }

private:
    JsonPath _path;
    std::vector<JsonSchemaError> _errors;
};

void check(const JsonValue &json, const JsonSchema &schema, ErrorContext &errors);

struct RequiredType
{
    JsonType value;

    bool accepts(JsonType type)
    {
        if (value == JsonType::Undefined)
        {
            return true;
        }
        if (type == value)
        {
            return true;
        }
        if (value == JsonType::Number && type == JsonType::Integer)
        {
            return true;
        }
        return false;
    }
};

JsonType getJsonType(const JsonValue &json)
{
    if (json.isEmpty())
    {
        return JsonType::Null;
    }
    if (json.isBoolean())
    {
        return JsonType::Boolean;
    }
    if (json.isInteger())
    {
        return JsonType::Integer;
    }
    if (json.isNumeric())
    {
        return JsonType::Number;
    }
    if (json.isString())
    {
        return JsonType::String;
    }
    if (isArray(json))
    {
        return JsonType::Array;
    }
    if (isObject(json))
    {
        return JsonType::Object;
    }
    throw std::invalid_argument("Value is not JSON");
}

void checkOneOf(const JsonValue &json, const JsonSchema &schema, ErrorContext &errors)
{
    for (const auto &oneof : schema.oneOf)
    {
        auto suberrors = validate(json, oneof);
        if (!suberrors.empty())
        {
            return;
        }
    }
    errors.add(InvalidOneOf{});
}

bool checkType(const JsonValue &json, const JsonSchema &schema, ErrorContext &errors)
{
    auto required = RequiredType{schema.type};
    auto type = getJsonType(json);
    if (required.accepts(type))
    {
        return true;
    }
    errors.add(InvalidType{type, required.value});
    return false;
}

void checkEnum(const std::string &value, const JsonSchema &schema, ErrorContext &errors)
{
    auto i = std::ranges::find(schema.enums, value);
    if (i != schema.enums.end())
    {
        return;
    }
    errors.add(InvalidEnum{value});
}

void checkRange(double value, const JsonSchema &schema, ErrorContext &errors)
{
    if (schema.minimum && value < *schema.minimum)
    {
        errors.add(BelowMinimum{value, *schema.minimum});
    }
    if (schema.maximum && value > *schema.maximum)
    {
        errors.add(AboveMaximum{value, *schema.maximum});
    }
}

void checkItemCount(std::size_t count, const JsonSchema &schema, ErrorContext &errors)
{
    if (schema.minItems && count < *schema.minItems)
    {
        errors.add(NotEnoughItems{count, *schema.minItems});
    }
    if (schema.maxItems && count > *schema.maxItems)
    {
        errors.add(TooManyItems{count, *schema.maxItems});
    }
}

void checkArrayItems(const JsonArray &array, const JsonSchema &schema, ErrorContext &errors)
{
    const auto &itemSchema = schema.items.at(0);

    auto index = std::size_t(0);
    for (const auto &value : array)
    {
        errors.push(index);
        check(value, itemSchema, errors);
        errors.pop();

        ++index;
    }
}

void checkMapItems(const JsonObject &object, const JsonSchema &schema, ErrorContext &errors)
{
    const auto &itemSchema = schema.items.at(0);

    for (const auto &[key, value] : object)
    {
        errors.push(key);
        check(value, itemSchema, errors);
        errors.pop();
    }
}

void checkRequiredProperties(const JsonObject &object, const JsonSchema &schema, ErrorContext &errors)
{
    for (const auto &[key, property] : schema.properties)
    {
        if (!property.required)
        {
            continue;
        }
        if (object.has(key))
        {
            continue;
        }
        errors.add(MissingRequiredProperty{key});
    }
}

void checkUnknownProperties(const JsonObject &object, const JsonSchema &schema, ErrorContext &errors)
{
    for (const auto &[key, value] : object)
    {
        if (schema.properties.contains(key))
        {
            continue;
        }
        errors.add(UnknownProperty{key});
    }
}

void checkProperties(const JsonObject &object, const JsonSchema &schema, ErrorContext &errors)
{
    for (const auto &[key, value] : object)
    {
        errors.push(key);
        check(value, schema.properties.at(key), errors);
        errors.pop();
    }
}

void checkObject(const JsonObject &object, const JsonSchema &schema, ErrorContext &errors)
{
    if (!schema.items.empty())
    {
        checkMapItems(object, schema, errors);
        return;
    }
    checkUnknownProperties(object, schema, errors);
    checkRequiredProperties(object, schema, errors);
    checkProperties(object, schema, errors);
}

void check(const JsonValue &json, const JsonSchema &schema, ErrorContext &errors)
{
    if (!schema.oneOf.empty())
    {
        checkOneOf(json, schema, errors);
        return;
    }
    if (!checkType(json, schema, errors))
    {
        return;
    }
    if (!schema.enums.empty())
    {
        const auto &value = json.extract<std::string>();
        checkEnum(value, schema, errors);
        return;
    }
    if (isNumeric(schema.type))
    {
        auto value = json.convert<double>();
        checkRange(value, schema, errors);
        return;
    }
    if (schema.type == JsonType::Array)
    {
        const auto &value = getArray(json);
        checkItemCount(value.size(), schema, errors);
        checkArrayItems(value, schema, errors);
        return;
    }
    if (schema.type == JsonType::Object)
    {
        const auto &object = getObject(json);
        checkObject(object, schema, errors);
        return;
    }
}
}

namespace brayns::experimental
{
std::string toString(const JsonPath &path)
{
    auto result = std::string();

    for (const auto &item : path)
    {
        const auto *index = std::get_if<std::size_t>(&item);

        if (index != nullptr)
        {
            result.append(fmt::format("[{}]", *index));
            continue;
        }

        const auto &key = std::get<std::string>(item);

        if (result.empty())
        {
            result.append(key);
            continue;
        }

        result.push_back('.');
        result.append(key);
    }

    return result;
}

std::string toString(const InvalidType &error)
{
    const auto &type = getEnumName(error.type);
    const auto &expected = getEnumName(error.expected);
    return fmt::format("Invalid type: expected {}, got {}", expected, type);
}

std::string toString(const BelowMinimum &error)
{
    return fmt::format("Value below minimum: {} < {}", error.value, error.minimum);
}

std::string toString(const AboveMaximum &error)
{
    return fmt::format("Value above maximum: {} > {}", error.value, error.maximum);
}

std::string toString(const NotEnoughItems &error)
{
    return fmt::format("Too many items: {} < {}", error.count, error.minItems);
}

std::string toString(const TooManyItems &error)
{
    return fmt::format("Too many items: {} > {}", error.count, error.maxItems);
}

std::string toString(const MissingRequiredProperty &error)
{
    return fmt::format("Missing required property: '{}'", error.name);
}

std::string toString(const UnknownProperty &error)
{
    return fmt::format("Unknown property: '{}'", error.name);
}

std::string toString(const InvalidEnum &error)
{
    return fmt::format("Invalid enum: '{}'", error.name);
}

std::string toString(const InvalidOneOf &)
{
    return "Invalid oneOf";
}

std::string toString(const JsonError &error)
{
    return std::visit([](const auto &value) { return toString(value); }, error);
}

std::vector<JsonSchemaError> validate(const JsonValue &json, const JsonSchema &schema)
{
    auto errors = ErrorContext();
    check(json, schema, errors);
    return errors.build();
}
}
