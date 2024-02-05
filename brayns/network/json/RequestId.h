/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include <typeindex>

#include <brayns/json/adapters/PrimitiveAdapter.h>

namespace brayns
{
/**
 * @brief Represent a JSON-RPC request ID.
 *
 * Can be null, integer or string. Being null means that no replies are needed.
 *
 */
class RequestId
{
public:
    RequestId() = default;

    RequestId(int64_t value)
        : _type(typeid(int64_t))
        , _int(value)
    {
    }

    RequestId(std::string value)
        : _type(typeid(std::string))
        , _string(std::move(value))
    {
    }

    bool isEmpty() const { return _type == typeid(void); }

    bool isInt() const { return _type == typeid(int64_t); }

    bool isString() const { return _type == typeid(std::string); }

    int64_t toInt() const { return _int; }

    const std::string& toString() const { return _string; }

    std::string getDisplayText() const
    {
        if (isInt())
        {
            return std::to_string(_int);
        }
        if (isString())
        {
            return "'" + _string + "'";
        }
        return {};
    }

    size_t getHashCode() const
    {
        auto hashCode = std::hash<std::type_index>()(_type);
        hashCode ^= std::hash<int64_t>()(_int);
        hashCode ^= std::hash<std::string>()(_string);
        return hashCode;
    }

    bool operator==(const RequestId& other) const
    {
        return _type == other._type && _int == other._int &&
               _string == other._string;
    }

    bool operator!=(const RequestId& other) const { return !(*this == other); }

private:
    std::type_index _type = typeid(void);
    int64_t _int = 0;
    std::string _string;
};

/**
 * @brief JSON interface for RequestId.
 *
 */
template <>
struct JsonAdapter<RequestId>
{
    static JsonSchema getSchema(const RequestId&)
    {
        JsonSchema schema;
        schema.title = "RequestId";
        schema.oneOf = {JsonSchemaHelper::getNullSchema(),
                        Json::getSchema<int64_t>(),
                        Json::getSchema<std::string>()};
        return schema;
    }

    static bool serialize(const RequestId& value, JsonValue& json)
    {
        if (value.isEmpty())
        {
            json.clear();
            return true;
        }
        if (value.isInt())
        {
            json = value.toInt();
            return true;
        }
        if (value.isString())
        {
            json = value.toString();
            return true;
        }
        return false;
    }

    static bool deserialize(const JsonValue& json, RequestId& value)
    {
        if (json.isEmpty())
        {
            value = {};
            return true;
        }
        if (json.isInteger() && !json.isBoolean())
        {
            value = json.convert<int64_t>();
            return true;
        }
        if (json.isString())
        {
            value = json.extract<std::string>();
            return true;
        }
        return false;
    }
};
} // namespace brayns

namespace std
{
template <>
struct hash<brayns::RequestId>
{
    size_t operator()(const brayns::RequestId& id) const
    {
        return id.getHashCode();
    }
};
} // namespace std
