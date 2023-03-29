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

#include "JsonTypes.h"

#include <stdexcept>

namespace brayns
{
EnumMap<JsonType> EnumReflector<JsonType>::reflect()
{
    return {
        {"undefined", JsonType::Undefined},
        {"null", JsonType::Null},
        {"boolean", JsonType::Boolean},
        {"integer", JsonType::Integer},
        {"number", JsonType::Number},
        {"string", JsonType::String},
        {"array", JsonType::Array},
        {"object", JsonType::Object}};
}

JsonType JsonTypeInfo::getType(const JsonValue &json)
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
    if (json.type() == typeid(JsonArray::Ptr))
    {
        return JsonType::Array;
    }
    if (json.type() == typeid(JsonObject::Ptr))
    {
        return JsonType::Object;
    }
    throw std::invalid_argument("Value is not a JSON type");
}

const JsonObject &JsonExtractor::extractObject(const JsonValue &json)
{
    return *json.extract<JsonObject::Ptr>();
}

const JsonArray &JsonExtractor::extractArray(const JsonValue &json)
{
    return *json.extract<JsonArray::Ptr>();
}
} // namespace brayns

namespace std
{
std::ostream &operator<<(std::ostream &stream, const brayns::JsonType &type)
{
    stream << brayns::EnumInfo::getName(type);
}
} // namespace std
