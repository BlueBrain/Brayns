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

namespace brayns
{
EnumInfo<JsonType> EnumReflector<JsonType>::reflect()
{
    auto builder = EnumBuilder<JsonType>();
    builder.field("undefined", JsonType::Undefined);
    builder.field("null", JsonType::Null);
    builder.field("boolean", JsonType::Boolean);
    builder.field("integer", JsonType::Integer);
    builder.field("number", JsonType::Number);
    builder.field("string", JsonType::String);
    builder.field("array", JsonType::Array);
    builder.field("object", JsonType::Object);
    return builder.build();
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