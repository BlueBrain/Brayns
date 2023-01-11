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

#include "JsonSchemaValidator.h"

#include <sstream>

#include <brayns/utils/string/StringJoiner.h>

namespace
{
using namespace brayns;

class JsonPath
{
public:
    bool isEmpty() const
    {
        return _path.empty();
    }

    void clear()
    {
        _path.clear();
    }

    void push(const std::string &key)
    {
        if (_path.empty())
        {
            _path.push_back(key);
            return;
        }
        _path.push_back("." + key);
    }

    void push(size_t index)
    {
        _path.push_back("[" + std::to_string(index) + "]");
    }

    void pop()
    {
        _path.pop_back();
    }

    std::string toString() const
    {
        return StringJoiner::join(_path, std::string_view());
    }

private:
    std::vector<std::string> _path;
};

class JsonValidatorContext
{
public:
    void clear()
    {
        _path.clear();
        _errors.clear();
    }

    void push(const std::string &key)
    {
        _path.push(key);
    }

    void push(size_t index)
    {
        _path.push(index);
    }

    void pop()
    {
        _path.pop();
    }

    const std::vector<std::string> &getErrors() const
    {
        return _errors;
    }

    void addError(std::string error)
    {
        _errors.push_back(std::move(error));
    }

    void addInvalidOneOf()
    {
        std::ostringstream stream;
        stream << "Cannot find a schema in oneOf";
        _addPath(stream);
        stream << " that matches the given input";
        addError(stream.str());
    }

    void addInvalidType(JsonType type, JsonType schemaType)
    {
        auto &typeName = EnumInfo::getName(type);
        auto &schemaTypeName = EnumInfo::getName(schemaType);
        addInvalidType(typeName, schemaTypeName);
    }

    void addInvalidType(const std::string &type, const std::string &schemaType)
    {
        std::ostringstream stream;
        stream << "Invalid type";
        _addPath(stream);
        stream << ": expected '" << schemaType << "', got '" << type << "'";
        addError(stream.str());
    }

    void addInvalidEnum(const JsonValue &json, const std::vector<std::string> &enums)
    {
        std::ostringstream stream;
        stream << "Invalid enum";
        _addPath(stream);
        stream << ": '" << json.toString() << "' not in [";
        if (!enums.empty())
        {
            stream << "'" << enums[0] << "'";
        }
        for (size_t i = 1; i < enums.size(); ++i)
        {
            stream << ", '" << enums[i] << "'";
        }
        stream << "]";
        addError(stream.str());
    }

    void addBelowMinimum(double value, double minimum)
    {
        std::ostringstream stream;
        stream << "Value below minimum";
        _addPath(stream);
        stream << ": expected >= " << minimum << ", got " << value;
        addError(stream.str());
    }

    void addAboveMaximum(double value, double maximum)
    {
        std::ostringstream stream;
        stream << "Value above maximum";
        _addPath(stream);
        stream << ": expected <= " << maximum << ", got " << value;
        addError(stream.str());
    }

    void addMissingProperty()
    {
        addError("Missing property: '" + _path.toString() + "'");
    }

    void addUnknownProperty()
    {
        addError("Unknown property: '" + _path.toString() + "'");
    }

    void addNotEnoughItems(size_t size, size_t minItems)
    {
        std::ostringstream stream;
        stream << "Not enough items";
        _addPath(stream);
        stream << ": expected at least " << minItems << " item(s), got " << size;
        addError(stream.str());
    }

    void addTooManyItems(size_t size, size_t maxItems)
    {
        std::ostringstream stream;
        stream << "Too many items";
        _addPath(stream);
        stream << ": expected at most " << maxItems << " item(s), got " << size;
        addError(stream.str());
    }

private:
    JsonPath _path;
    std::vector<std::string> _errors;

    void _addPath(std::ostream &stream) const
    {
        if (_path.isEmpty())
        {
            return;
        }
        stream << " for " << _path.toString();
    }
};

class JsonValidator
{
public:
    std::vector<std::string> validate(const JsonValue &json, const JsonSchema &schema)
    {
        _context.clear();
        _validate(json, schema);
        return _context.getErrors();
    }

private:
    JsonValidatorContext _context;

    void _validate(const JsonValue &json, const JsonSchema &schema)
    {
        if (JsonSchemaHelper::isWildcard(schema))
        {
            return;
        }
        if (JsonSchemaHelper::isOneOf(schema))
        {
            _validateOneOf(json, schema);
            return;
        }
        if (!_validateType(json, schema))
        {
            return;
        }
        if (JsonSchemaHelper::isNumeric(schema))
        {
            _validateLimits(json, schema);
            return;
        }
        if (JsonSchemaHelper::isEnum(schema))
        {
            _validateEnum(json, schema);
            return;
        }
        if (JsonSchemaHelper::isObject(schema))
        {
            _validateProperties(json, schema);
            _validateAdditionalProperties(json, schema);
            return;
        }
        if (JsonSchemaHelper::isArray(schema))
        {
            _validateItems(json, schema);
        }
    }

    void _validateOneOf(const JsonValue &json, const JsonSchema &schema)
    {
        auto backup = std::move(_context);
        for (const auto &oneOf : schema.oneOf)
        {
            _context.clear();
            _validate(json, oneOf);
            auto &errors = _context.getErrors();
            if (errors.empty())
            {
                _context = std::move(backup);
                return;
            }
        }
        _context = std::move(backup);
        _context.addInvalidOneOf();
    }

    bool _validateType(const JsonValue &json, const JsonSchema &schema)
    {
        auto type = GetJsonType::fromJson(json);
        if (!JsonSchemaHelper::checkType(schema, type))
        {
            _context.addInvalidType(type, schema.type);
            return false;
        }
        return true;
    }

    void _validateLimits(const JsonValue &json, const JsonSchema &schema)
    {
        auto value = json.convert<double>();
        if (schema.minimum && value < *schema.minimum)
        {
            _context.addBelowMinimum(value, *schema.minimum);
            return;
        }
        if (schema.maximum && value > *schema.maximum)
        {
            _context.addAboveMaximum(value, *schema.maximum);
        }
    }

    void _validateEnum(const JsonValue &json, const JsonSchema &schema)
    {
        const auto &value = json.extract<std::string>();
        for (const auto &item : schema.enums)
        {
            if (value == item)
            {
                return;
            }
        }
        _context.addInvalidEnum(json, schema.enums);
    }

    void _validateProperties(const JsonValue &json, const JsonSchema &schema)
    {
        auto &object = json.extract<JsonObject::Ptr>();
        for (const auto &pair : schema.properties)
        {
            auto &name = pair.first;
            _context.push(name);
            _validateProperty(name, object, schema);
            _context.pop();
        }
    }

    void _validateProperty(const std::string &name, const JsonObject::Ptr &object, const JsonSchema &schema)
    {
        auto json = object->get(name);
        if (!json.isEmpty())
        {
            _validate(json, schema.properties.at(name));
            return;
        }
        if (!JsonSchemaHelper::isRequired(schema, name))
        {
            return;
        }
        _context.addMissingProperty();
    }

    void _validateAdditionalProperties(const JsonValue &json, const JsonSchema &schema)
    {
        auto &object = *json.extract<JsonObject::Ptr>();
        for (const auto &pair : object)
        {
            auto &name = pair.first;
            auto &child = pair.second;
            _context.push(name);
            _validateAdditionalProperty(name, child, schema);
            _context.pop();
        }
    }

    void _validateAdditionalProperty(const std::string &name, const JsonValue &json, const JsonSchema &schema)
    {
        if (JsonSchemaHelper::hasProperty(schema, name))
        {
            return;
        }
        auto &additionalProperties = schema.additionalProperties;
        if (additionalProperties.empty())
        {
            _context.addUnknownProperty();
            return;
        }
        _validate(json, additionalProperties[0]);
    }

    void _validateItems(const JsonValue &json, const JsonSchema &schema)
    {
        if (schema.items.empty())
        {
            return;
        }
        auto &array = *json.extract<JsonArray::Ptr>();
        _validateItemLimits(array.size(), schema);
        _validateItems(array, schema);
    }

    void _validateItemLimits(size_t size, const JsonSchema &schema)
    {
        if (schema.minItems && size < *schema.minItems)
        {
            _context.addNotEnoughItems(size, *schema.minItems);
            return;
        }
        size_t max = std::numeric_limits<unsigned int>::max();
        if (size > max)
        {
            _context.addTooManyItems(size, max);
        }
        if (schema.maxItems && size > *schema.maxItems)
        {
            _context.addTooManyItems(size, *schema.maxItems);
        }
    }

    void _validateItems(const JsonArray &array, const JsonSchema &schema)
    {
        size_t i = 0;
        for (const auto &item : array)
        {
            _context.push(i);
            _validate(item, schema.items[0]);
            _context.pop();
            ++i;
        }
    }
};
} // namespace

namespace brayns
{
std::vector<std::string> JsonSchemaValidator::validate(const JsonValue &json, const JsonSchema &schema)
{
    JsonValidator validator;
    return validator.validate(json, schema);
}
} // namespace brayns
