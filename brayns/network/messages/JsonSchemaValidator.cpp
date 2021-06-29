/* Copyright (c) 2021 EPFL/Blue Brain Project
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

using namespace brayns;

namespace
{
class JsonPath
{
public:
    void clear() { _path.clear(); }

    void push(const std::string& key)
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

    void pop() { _path.pop_back(); }

    std::string toString() const { return string_utils::join(_path, {}); }

private:
    std::vector<std::string> _path;
};

class JsonValidatorContext
{
public:
    void push(const std::string& key) { _path.push(key); }

    void push(size_t index) { _path.push(index); }

    void pop() { _path.pop(); }

    const std::vector<std::string>& getErrors() const { return _errors; }

    void addInvalidType(const std::string& type, const std::string& schemaType)
    {
        std::ostringstream stream;
        stream << "Invalid type";
        auto path = _path.toString();
        if (!path.empty())
        {
            stream << " at '" << path << "'";
        }
        stream << ": expected '" << schemaType << "' got '" << type << "'";
        _errors.push_back(stream.str());
    }

    void addBelowMinimum(double value, double minimum)
    {
        std::ostringstream stream;
        stream << "'" << _path.toString() << "' is below minimum value '"
               << minimum << "'";
        _errors.push_back(stream.str());
    }

    void addAboveMaximum(double value, double maximum)
    {
        std::ostringstream stream;
        stream << "'" << _path.toString() << "' is above maximum value '"
               << maximum << "'";
        _errors.push_back(stream.str());
    }

    void addMissingProperty()
    {
        _errors.push_back("Missing property: '" + _path.toString() + "'");
    }

    void addNotEnoughItems(size_t size, size_t minItems)
    {
        std::ostringstream stream;
        stream << "Not enough items in '" << _path.toString()
               << "': min '" << minItems << "' got '" << size << "'";
        _errors.push_back(stream.str());
    }

    void addTooMuchItems(size_t size, size_t maxItems)
    {
        std::ostringstream stream;
        stream << "Too much items in '" << _path.toString()
               << "': max '" << maxItems << "' got '" << size << "'";
        _errors.push_back(stream.str());
    }

private:
    JsonPath _path;
    std::vector<std::string> _errors;
};

class JsonValidator
{
public:
    JsonSchemaErrorList validate(const JsonValue& json,
                                 const JsonSchema& schema)
    {
        _context = {};
        _validate(json, schema);
        return _context.getErrors();
    }

private:
    void _validate(const JsonValue& json, const JsonSchema& schema)
    {
        if (JsonSchemaInfo::isEmpty(schema))
        {
            return;
        }
        if (!_validateType(json, schema))
        {
            return;
        }
        if (JsonSchemaInfo::isNumber(schema))
        {
            _validateLimits(json, schema);
            return;
        }
        if (JsonSchemaInfo::isObject(schema))
        {
            _validateProperties(json, schema);
            return;
        }
        if (JsonSchemaInfo::isArray(schema))
        {
            _validateItems(json, schema);
        }
    }

    bool _validateType(const JsonValue& json, const JsonSchema& schema)
    {
        auto& type = JsonValueType::of(json);
        if (!JsonSchemaInfo::hasType(schema, type))
        {
            _context.addInvalidType(type, schema.type);
            return false;
        }
        return true;
    }

    void _validateLimits(const JsonValue& json, const JsonSchema& schema)
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

    void _validateProperties(const JsonValue& json, const JsonSchema& schema)
    {
        auto& object = *json.extract<JsonObject::Ptr>();
        for (const auto& property : schema.properties)
        {
            _context.push(property.first);
            _validateProperty(property, object, schema);
            _context.pop();
        }
    }

    void _validateProperty(const JsonProperty& property,
                           const JsonObject& object, const JsonSchema& schema)
    {
        auto json = object.get(property.first);
        if (!json.isEmpty())
        {
            _validate(json, property.second);
            return;
        }
        if (JsonSchemaInfo::isRequired(schema, property.first))
        {
            _context.addMissingProperty();
        }
    }

    void _validateItems(const JsonValue& json, const JsonSchema& schema)
    {
        if (schema.items.empty())
        {
            return;
        }
        auto& array = *json.extract<JsonArray::Ptr>();
        _validateItemLimits(array.size(), schema);
        for (size_t i = 0; i < array.size(); ++i)
        {
            _context.push(i);
            _validate(array.get(i), schema.items[0]);
            _context.pop();
        }
    }

    void _validateItemLimits(size_t size, const JsonSchema& schema)
    {
        if (schema.minItems && size < *schema.minItems)
        {
            _context.addNotEnoughItems(size, *schema.minItems);
            return;
        }
        if (schema.maxItems && size > *schema.maxItems)
        {
            _context.addTooMuchItems(size, *schema.maxItems);
        }
    }

    JsonValidatorContext _context;
};
} // namespace

namespace brayns
{
JsonSchemaErrorList JsonSchemaValidator::validate(const JsonValue& json,
                                                  const JsonSchema& schema)
{
    JsonValidator validator;
    return validator.validate(json, schema);
}
} // namespace brayns