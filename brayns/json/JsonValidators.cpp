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

#include "JsonValidators.h"

#include <sstream>

#include <brayns/utils/EnumInfo.h>

namespace
{
class JsonTypeChecker
{
public:
    static bool check(brayns::JsonType type, brayns::JsonType required)
    {
        if (type == required)
        {
            return true;
        }
        if (required == brayns::JsonType::Undefined)
        {
            return true;
        }
        if (required == brayns::JsonType::Number && type == brayns::JsonType::Integer)
        {
            return true;
        }
        return false;
    }

    static bool check(brayns::JsonType type, brayns::JsonType required, brayns::JsonErrors &errors)
    {
        if (check(type, required))
        {
            return true;
        }
        errors.add("invalid type: expected {}, got {}", required, type);
        return false;
    }

    static bool check(const brayns::JsonValue &json, brayns::JsonType required, brayns::JsonErrors &errors)
    {
        auto type = brayns::JsonTypeInfo::getType(json);
        return check(type, required, errors);
    }
};

class JsonRangeChecker
{
public:
    static void check(double value, double min, double max, brayns::JsonErrors &errors)
    {
        if (value < min)
        {
            errors.add("value below minimum: {} < {}", value, min);
        }
        if (value > max)
        {
            errors.add("value above maximum: {} > {}", value, max);
        }
    }
};

class JsonArrayChecker
{
public:
    static void checkItems(const brayns::JsonArray &array, const brayns::JsonSchema &schema, brayns::JsonErrors &errors)
    {
        auto i = size_t(0);
        for (const auto &child : array)
        {
            errors.push(i);
            schema.validate(child, errors);
            errors.pop();
            ++i;
        }
    }

    static void checkItemCount(size_t size, size_t min, size_t max, brayns::JsonErrors &errors)
    {
        if (size < min)
        {
            errors.add("item count below minimum: {} < {}", size, min);
        }
        if (size > max)
        {
            errors.add("item count above maximum: {} > {}", size, max);
        }
    }
};

class JsonObjectChecker
{
public:
    static void checkItems(
        const brayns::JsonObject &object,
        const std::map<std::string, brayns::JsonSchema> &properties,
        brayns::JsonErrors &errors)
    {
        for (const auto &[key, child] : object)
        {
            auto i = properties.find(key);
            if (i == properties.end())
            {
                errors.add("unknown property: '{}'", key);
                continue;
            }
            errors.push(key);
            auto &items = i->second;
            items.validate(child, errors);
            errors.pop();
        }
    }

    static void checkRequired(
        const brayns::JsonObject &json,
        const std::vector<std::string> &required,
        brayns::JsonErrors &errors)
    {
        for (const auto &key : required)
        {
            if (json.has(key))
            {
                continue;
            }
            errors.add("missing required property: '{}'", key);
        }
    }
};

class JsonEnumChecker
{
public:
    static void check(const std::string &value, const std::vector<std::string> &values, brayns::JsonErrors &errors)
    {
        if (std::find(values.begin(), values.end(), value) != values.end())
        {
            return;
        }
        auto stream = std::ostringstream();
        if (!values.empty())
        {
            stream << "'" << values[0] << "'";
        }
        for (const auto &item : values)
        {
            stream << ", '" << item << "'";
        }
        errors.add("invalid enum: '{}' not in [{}]", value, stream.str());
    }
};
} // namespace

namespace brayns
{
void JsonSchemaValidator<WildcardSchema>::validate(
    const JsonValue &json,
    const WildcardSchema &schema,
    JsonErrors &errors)
{
    (void)json;
    (void)schema;
    (void)errors;
}

void JsonSchemaValidator<NullSchema>::validate(const JsonValue &json, const NullSchema &schema, JsonErrors &errors)
{
    (void)schema;
    JsonTypeChecker::check(json, JsonType::Null, errors);
}

void JsonSchemaValidator<BooleanSchema>::validate(
    const JsonValue &json,
    const BooleanSchema &schema,
    JsonErrors &errors)
{
    (void)schema;
    JsonTypeChecker::check(json, JsonType::Boolean, errors);
}

void JsonSchemaValidator<IntegerSchema>::validate(
    const JsonValue &json,
    const IntegerSchema &schema,
    JsonErrors &errors)
{
    if (!JsonTypeChecker::check(json, JsonType::Integer, errors))
    {
        return;
    }
    auto value = json.convert<double>();
    JsonRangeChecker::check(value, schema.min, schema.max, errors);
}

void JsonSchemaValidator<NumberSchema>::validate(const JsonValue &json, const NumberSchema &schema, JsonErrors &errors)
{
    if (!JsonTypeChecker::check(json, JsonType::Number, errors))
    {
        return;
    }
    auto value = json.convert<double>();
    JsonRangeChecker::check(value, schema.min, schema.max, errors);
}

void JsonSchemaValidator<StringSchema>::validate(const JsonValue &json, const StringSchema &schema, JsonErrors &errors)
{
    (void)schema;
    JsonTypeChecker::check(json, JsonType::String, errors);
}

void JsonSchemaValidator<ArraySchema>::validate(const JsonValue &json, const ArraySchema &schema, JsonErrors &errors)
{
    if (!JsonTypeChecker::check(json, JsonType::Array, errors))
    {
        return;
    }
    auto &array = JsonExtractor::extractArray(json);
    JsonArrayChecker::checkItemCount(array.size(), schema.minItems, schema.maxItems, errors);
    JsonArrayChecker::checkItems(array, schema.items, errors);
}

void JsonSchemaValidator<MapSchema>::validate(const JsonValue &json, const MapSchema &schema, JsonErrors &errors)
{
    if (!JsonTypeChecker::check(json, JsonType::Object, errors))
    {
        return;
    }
    auto &object = JsonExtractor::extractObject(json);
    for (const auto &[key, child] : object)
    {
        errors.push(key);
        schema.items.validate(child, errors);
        errors.pop();
    }
}

void JsonSchemaValidator<ObjectSchema>::validate(const JsonValue &json, const ObjectSchema &schema, JsonErrors &errors)
{
    if (!JsonTypeChecker::check(json, JsonType::Object, errors))
    {
        return;
    }
    auto &object = JsonExtractor::extractObject(json);
    JsonObjectChecker::checkRequired(object, schema.required, errors);
    JsonObjectChecker::checkItems(object, schema.properties, errors);
}

void JsonSchemaValidator<EnumSchema>::validate(const JsonValue &json, const EnumSchema &schema, JsonErrors &errors)
{
    if (!JsonTypeChecker::check(json, JsonType::String, errors))
    {
        return;
    }
    auto &value = json.extract<std::string>();
    JsonEnumChecker::check(value, schema.values, errors);
}

void JsonSchemaValidator<OneOfSchema>::validate(const JsonValue &json, const OneOfSchema &schema, JsonErrors &errors)
{
    for (const auto &oneOf : schema.schemas)
    {
        auto mismatch = JsonErrors();
        oneOf.validate(json, errors);
        if (mismatch.isEmpty())
        {
            return;
        }
    }
    errors.add("invalid oneOf");
}
} // namespace brayns
