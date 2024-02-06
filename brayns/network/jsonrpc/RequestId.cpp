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

#include "RequestId.h"

#include <brayns/json/Json.h>
#include <brayns/json/adapters/PrimitiveAdapter.h>

namespace brayns
{
RequestId::RequestId(int64_t value):
    _type(typeid(int64_t)),
    _int(value)
{
}

RequestId::RequestId(std::string value):
    _type(typeid(std::string)),
    _string(std::move(value))
{
}

bool RequestId::isEmpty() const
{
    return _type == typeid(void);
}

bool RequestId::isInt() const
{
    return _type == typeid(int64_t);
}

bool RequestId::isString() const
{
    return _type == typeid(std::string);
}

int64_t RequestId::toInt() const
{
    return _int;
}

const std::string &RequestId::toString() const
{
    return _string;
}

std::string RequestId::getDisplayText() const
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

size_t RequestId::getHashCode() const
{
    auto hashCode = std::hash<std::type_index>()(_type);
    hashCode ^= std::hash<int64_t>()(_int);
    hashCode ^= std::hash<std::string>()(_string);
    return hashCode;
}

bool RequestId::operator==(const RequestId &other) const
{
    return _type == other._type && _int == other._int && _string == other._string;
}

bool RequestId::operator!=(const RequestId &other) const
{
    return !(*this == other);
}

JsonSchema JsonAdapter<RequestId>::getSchema()
{
    JsonSchema schema;
    schema.title = "RequestId";
    schema.oneOf = {Json::getSchema<EmptyJson>(), Json::getSchema<int64_t>(), Json::getSchema<std::string>()};
    return schema;
}

void JsonAdapter<RequestId>::serialize(const RequestId &value, JsonValue &json)
{
    if (value.isInt())
    {
        json = value.toInt();
        return;
    }
    if (value.isString())
    {
        json = value.toString();
        return;
    }
}

void JsonAdapter<RequestId>::deserialize(const JsonValue &json, RequestId &value)
{
    if (json.isEmpty())
    {
        value = {};
        return;
    }
    if (json.isInteger() && !json.isBoolean())
    {
        auto integer = json.convert<int64_t>();
        value = RequestId(integer);
        return;
    }
    if (json.isString())
    {
        auto string = json.extract<std::string>();
        value = RequestId(string);
        return;
    }
}
} // namespace brayns
