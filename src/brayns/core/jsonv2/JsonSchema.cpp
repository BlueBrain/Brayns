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

#include "JsonSchema.h"

#include <stdexcept>

namespace brayns::experimental
{
EnumInfo<JsonType> EnumReflector<JsonType>::reflect()
{
    return {
        {"undefined", JsonType::Undefined},
        {"null", JsonType::Null},
        {"boolean", JsonType::Boolean},
        {"integer", JsonType::Integer},
        {"number", JsonType::Number},
        {"string", JsonType::String},
        {"array", JsonType::Array},
        {"object", JsonType::Object},
    };
}

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
    throw JsonException("Value is not JSON");
}

void RequiredJsonType::throwIfNotCompatible(JsonType type)
{
    if (!isCompatible(type))
    {
        throw JsonException("Incompatible JSON types");
    }
}
}
