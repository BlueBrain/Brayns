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

#pragma once

#include <sstream>
#include <string>
#include <vector>

#include <brayns/common/utils/stringUtils.h>

#include "JsonSchema.h"

namespace brayns
{
/**
 * @brief Helper class to store the path of a JSON member.
 *
 */
class JsonPath
{
public:
    std::string join() const { return string_utils::join(_path, "."); }

    void clear() { _path.clear(); }

    void push(const std::string& key) { _path.push_back(key); }

    void pop() { _path.pop_back(); }

private:
    std::vector<std::string> _path;
};

/**
 * @brief Helper class to store JSON schema validation errors.
 *
 */
class JsonSchemaErrors
{
public:
    bool isEmpty() const { return _errors.empty(); }

    std::string join() const
    {
        return "- " + string_utils::join(_errors, "\n- ");
    }

    void clear() { _errors.clear(); }

    void add(const std::string& error) { _errors.push_back(error); }

    void addTypeError(const std::string& path, const std::string& schemaType,
                      const std::string& type)
    {
        std::ostringstream stream;
        stream << "Invalid type";
        if (!path.empty())
        {
            stream << " at '" << path << '\'';
        }
        stream << ": expected '" << schemaType << "' got '" << type << "'";
        add(stream.str());
    }

    void addMissingProperty(const std::string& path)
    {
        add("Missing required property: '" + path + "'");
    }

private:
    std::vector<std::string> _errors;
};

/**
 * @brief Validate a JSON value from a JSON schema.
 *
 */
class JsonSchemaValidator
{
public:
    /**
     * @brief Validate json using schema.
     *
     * @param json JSON value to control.
     * @param schema JSON schema used.
     * @return true Value respect the schema.
     * @return false Value doesn't respect the schema.
     */
    bool validate(const JsonValue& json, const JsonSchema& schema)
    {
        _path.clear();
        _errors.clear();
        _validate(json, schema);
        return _errors.isEmpty();
    }

    /**
     * @brief Get a description of the errors if the last validated JSON value
     * was incorrect.
     *
     * @return std::string Human readable aggregated error messages.
     */
    std::string getErrorsDescription() const { return _errors.join(); }

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
            _errors.addTypeError(_path.join(), schema.type, type);
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
            _path.push(property.first);
            auto child = object.get(property.first);
            if (!child.isEmpty())
            {
                _validate(child, property.second);
                _path.pop();
                continue;
            }
            if (schema.requires(property.first))
            {
                _errors.addMissingProperty(_path.join());
            }
            _path.pop();
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
        std::ostringstream stream;
        for (size_t i = 0; i < array.size(); ++i)
        {
            stream << '[' << i << ']';
            _path.push(stream.str());
            _validate(array.get(i), schema);
            _path.pop();
            stream.str("");
        }
    }

    JsonPath _path;
    JsonSchemaErrors _errors;
};
} // namespace brayns