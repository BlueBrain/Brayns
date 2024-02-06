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

#include <sstream>

#include <brayns/utils/EnumInfo.h>

namespace
{
class TypeValidator
{
public:
    static bool validate(const brayns::JsonValue &json, brayns::JsonType required, brayns::JsonErrorBuilder &errors)
    {
        auto type = brayns::JsonTypeInfo::getType(json);
        return _validate(type, required, errors);
    }

private:
    static bool _validate(brayns::JsonType type, brayns::JsonType required)
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

    static bool _validate(brayns::JsonType type, brayns::JsonType required, brayns::JsonErrorBuilder &errors)
    {
        if (_validate(type, required))
        {
            return true;
        }
        errors.add("invalid type, expected {} got {}", required, type);
        return false;
    }
};

class NumberValidator
{
public:
    static void validate(
        const brayns::JsonValue &json,
        const brayns::JsonSchema &schema,
        brayns::JsonErrorBuilder &errors)
    {
        auto value = json.convert<double>();
        checkRange(value, schema, errors);
    }

    static void checkRange(double value, const brayns::JsonSchema &schema, brayns::JsonErrorBuilder &errors)
    {
        if (schema.minimum && value < *schema.minimum)
        {
            errors.add("value below minimum {} < {}", value, *schema.minimum);
        }
        if (schema.maximum && value > *schema.maximum)
        {
            errors.add("value above maximum {} > {}", value, *schema.maximum);
        }
    }
};

class ArrayValidator
{
public:
    static void validate(
        const brayns::JsonValue &json,
        const brayns::JsonSchema &schema,
        brayns::JsonErrorBuilder &errors)
    {
        if (schema.items.empty())
        {
            throw std::invalid_argument("Invalid array schema without items");
        }
        auto &array = brayns::JsonExtractor::extractArray(json);
        checkItemSchema(array, schema.items[0], errors);
        checkItemCount(array.size(), schema, errors);
    }

    static void checkItemSchema(
        const brayns::JsonArray &array,
        const brayns::JsonSchema &schema,
        brayns::JsonErrorBuilder &errors)
    {
        auto i = size_t(0);
        for (const auto &item : array)
        {
            errors.push(i);
            brayns::JsonValidator::validate(item, schema, errors);
            errors.pop();
            ++i;
        }
    }

    static void checkItemCount(size_t size, const brayns::JsonSchema &schema, brayns::JsonErrorBuilder &errors)
    {
        if (schema.minItems && size < *schema.minItems)
        {
            errors.add("item count below minimum {} < {}", size, *schema.minItems);
        }
        if (schema.maxItems && size > *schema.maxItems)
        {
            errors.add("item count above maximum {} > {}", size, *schema.maxItems);
        }
    }
};

class MapValidator
{
public:
    static void validate(
        const brayns::JsonValue &json,
        const brayns::JsonSchema &schema,
        brayns::JsonErrorBuilder &errors)
    {
        if (schema.items.empty())
        {
            throw std::invalid_argument("Invalid map schema without items");
        }
        auto &object = brayns::JsonExtractor::extractObject(json);
        checkItemSchema(object, schema.items[0], errors);
    }

    static void checkItemSchema(
        const brayns::JsonObject &object,
        const brayns::JsonSchema &schema,
        brayns::JsonErrorBuilder &errors)
    {
        for (const auto &[key, value] : object)
        {
            errors.push(key);
            brayns::JsonValidator::validate(value, schema, errors);
            errors.pop();
        }
    }
};

class ObjectValidator
{
public:
    static void validate(
        const brayns::JsonValue &json,
        const brayns::JsonSchema &schema,
        brayns::JsonErrorBuilder &errors)
    {
        auto &object = brayns::JsonExtractor::extractObject(json);
        auto &properties = schema.properties;
        checkPropertiesSchema(object, properties, errors);
        checkRequired(object, properties, errors);
    }

    static void checkPropertiesSchema(
        const brayns::JsonObject &object,
        const std::map<std::string, brayns::JsonSchema> &properties,
        brayns::JsonErrorBuilder &errors)
    {
        for (const auto &[key, value] : object)
        {
            auto i = properties.find(key);
            if (i == properties.end())
            {
                errors.add("unknown property '{}'", key);
                continue;
            }
            auto &schema = i->second;
            if (schema.readOnly)
            {
                errors.add("read only property '{}'", key);
                continue;
            }
            errors.push(key);
            brayns::JsonValidator::validate(value, schema, errors);
            errors.pop();
        }
    }

    static void checkRequired(
        const brayns::JsonObject &json,
        const std::map<std::string, brayns::JsonSchema> &properties,
        brayns::JsonErrorBuilder &errors)
    {
        for (const auto &[key, value] : properties)
        {
            if (!value.required)
            {
                continue;
            }
            if (value.readOnly)
            {
                continue;
            }
            if (json.has(key))
            {
                continue;
            }
            errors.add("missing required property '{}'", key);
        }
    }
};

class EnumValidator
{
public:
    static void validate(
        const brayns::JsonValue &json,
        const brayns::JsonSchema &schema,
        brayns::JsonErrorBuilder &errors)
    {
        auto &value = json.extract<std::string>();
        checkValues(value, schema.enums, errors);
    }

    static void checkValues(
        const std::string &value,
        const std::vector<std::string> &values,
        brayns::JsonErrorBuilder &errors)
    {
        if (std::find(values.begin(), values.end(), value) != values.end())
        {
            return;
        }
        auto stream = std::ostringstream();
        stream << '[';
        if (!values.empty())
        {
            stream << "'" << values[0] << "'";
        }
        for (size_t i = 1; i < values.size(); ++i)
        {
            stream << ", '" << values[i] << "'";
        }
        stream << ']';
        errors.add("invalid enum '{}' not in {}", value, stream.str());
    }
};

class OneOfValidator
{
public:
    static void validate(
        const brayns::JsonValue &json,
        const brayns::JsonSchema &schema,
        brayns::JsonErrorBuilder &errors)
    {
        checkAtLeastOneSchemaMatches(json, schema.oneOf, errors);
    }

    static void checkAtLeastOneSchemaMatches(
        const brayns::JsonValue &json,
        const std::vector<brayns::JsonSchema> &schemas,
        brayns::JsonErrorBuilder &errors)
    {
        for (const auto &schema : schemas)
        {
            auto builder = brayns::JsonErrorBuilder();
            brayns::JsonValidator::validate(json, schema, builder);
            auto buffer = builder.build();
            if (buffer.empty())
            {
                return;
            }
        }
        errors.add("invalid oneOf, no schemas match input");
    }
};

class ValidationDispatcher
{
public:
    static void validate(
        const brayns::JsonValue &json,
        const brayns::JsonSchema &schema,
        brayns::JsonErrorBuilder &errors)
    {
        if (!schema.oneOf.empty())
        {
            OneOfValidator::validate(json, schema, errors);
            return;
        }
        if (!TypeValidator::validate(json, schema.type, errors))
        {
            return;
        }
        if (brayns::JsonTypeInfo::isNumeric(schema.type))
        {
            NumberValidator::validate(json, schema, errors);
            return;
        }
        if (!schema.enums.empty())
        {
            EnumValidator::validate(json, schema, errors);
            return;
        }
        if (schema.type == brayns::JsonType::Array)
        {
            ArrayValidator::validate(json, schema, errors);
            return;
        }
        if (schema.type == brayns::JsonType::Object)
        {
            _validateObject(json, schema, errors);
            return;
        }
    }

private:
    static void _validateObject(
        const brayns::JsonValue &json,
        const brayns::JsonSchema &schema,
        brayns::JsonErrorBuilder &errors)
    {
        if (!schema.items.empty())
        {
            MapValidator::validate(json, schema, errors);
            return;
        }
        ObjectValidator::validate(json, schema, errors);
    }
};
} // namespace

namespace brayns
{
void JsonValidator::validate(const JsonValue &json, const JsonSchema &schema, JsonErrorBuilder &errors)
{
    if (json.isEmpty())
    {
        ValidationDispatcher::validate(schema.defaultValue, schema, errors);
        return;
    }
    ValidationDispatcher::validate(json, schema, errors);
}
} // namespace brayns
