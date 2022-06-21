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

#include "JsonType.h"

namespace brayns
{
bool JsonTypeHelper::check(JsonType required, JsonType type)
{
    return required == type || (required == JsonType::Number && type == JsonType::Integer);
}

bool JsonTypeHelper::isNumeric(JsonType type)
{
    return type == JsonType::Integer || type == JsonType::Number;
}

bool JsonTypeHelper::isPrimitive(JsonType type)
{
    return type > JsonType::Undefined && type <= JsonType::String;
}

bool JsonTypeHelper::isPrimitive(const JsonValue &json)
{
    return json.isNumeric() || json.isString();
}

JsonType GetJsonType::fromJson(const JsonValue &json)
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
    return JsonType::Undefined;
}
} // namespace brayns
