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

    void push(const std::string& key) { _path.push_back(key); }

    void pop() { _path.pop_back(); }

    std::string toString() const
    {
        if (_path.empty())
        {
            return {};
        }
        std::ostringstream stream;
        stream << _path[0];
        for (size_t i = 1; i < _path.size(); ++i)
        {
            auto& key = _path[i];
            if (key.empty() || key[0] != '[')
            {
                stream << '.';
            }
            stream << key;
        }
        return stream.str();
    }

private:
    std::vector<std::string> _path;
};

class JsonValidatorContext
{
public:
    void push(const std::string& key) { _path.push(key); }

    void push(size_t index)
    {
        std::ostringstream stream;
        stream << '[' << index << ']';
        push(stream.str());
    }

    void pop() { _path.pop(); }

    const std::vector<std::string>& getErrors() const { return _errors; }

    void addInvalidType(const std::string& type, const std::string& schemaType)
    {
        std::ostringstream stream;
        stream << "Invalid type";
        auto path = _path.toString();
        if (!path.empty())
        {
            stream << " at " << path;
        }
        stream << ": expected " << schemaType << " got " << type;
        _errors.push_back(stream.str());
    }

    void addMissingProperty()
    {
        _errors.push_back("Missing required property: " + _path.toString());
    }

private:
    JsonPath _path;
    std::vector<std::string> _errors;
};

class JsonValidator
{
public:
    JsonSchemaErrorList validate(const JsonValue& json, const JsonSchema& schema)
    {
        _context = {};
        _validate(json, schema);
        return _context.getErrors();
    }

private:
    void _validate(const JsonValue& json, const JsonSchema& schema)
    {
        if (!_validateType(json, schema))
        {
            return;
        }
        if (_validateProperties(json, schema))
        {
            return;
        }
        _validateItems(json, schema);
    }

    bool _validateType(const JsonValue& json, const JsonSchema& schema)
    {
        if (schema.type.empty())
        {
            return false;
        }
        auto& type = JsonTypeHelper::getJsonTypeName(json);
        if (type != schema.type)
        {
            _context.addInvalidType(type, schema.type);
            return false;
        }
        return true;
    }

    bool _validateProperties(const JsonValue& json, const JsonSchema& schema)
    {
        if (schema.type != JsonTypeName::ofObject())
        {
            return false;
        }
        if (schema.properties.empty())
        {
            return false;
        }
        if (json.type() != typeid(JsonObject::Ptr))
        {
            return false;
        }
        auto& object = *json.extract<JsonObject::Ptr>();
        _validateProperties(object, schema);
        return true;
    }

    void _validateProperties(const JsonObject& object, const JsonSchema& schema)
    {
        for (const auto& property : schema.properties)
        {
            _context.push(property.first);
            auto child = object.get(property.first);
            if (!child.isEmpty())
            {
                _validate(child, property.second);
                _context.pop();
                continue;
            }
            if (schema.requires(property.first))
            {
                _context.addMissingProperty();
            }
            _context.pop();
        }
    }

    bool _validateItems(const JsonValue& json, const JsonSchema& schema)
    {
        if (schema.type != JsonTypeName::ofArray())
        {
            return false;
        }
        if (schema.items.empty())
        {
            return false;
        }
        if (json.type() != typeid(JsonArray::Ptr))
        {
            return false;
        }
        auto& array = *json.extract<JsonArray::Ptr>();
        _validateItems(array, schema.items[0]);
        return true;
    }

    void _validateItems(const JsonArray& array, const JsonSchema& schema)
    {
        for (size_t i = 0; i < array.size(); ++i)
        {
            _context.push(i);
            _validate(array.get(i), schema);
            _context.pop();
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