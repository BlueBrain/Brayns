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

#pragma once

#include "JsonSchema.h"
#include "JsonValue.h"

namespace brayns::experimental
{
template<typename T>
struct JsonReflector
{
    template<typename U>
    static constexpr auto alwaysFalse = false;

    static_assert(alwaysFalse<T>, "Please specialize JsonReflector<T>");

    static JsonSchema getSchema()
    {
        return {};
    }

    static JsonValue serialize(const T &value)
    {
        return {};
    }

    static T deserialize(const JsonValue &json)
    {
        return {};
    }
};

template<typename T>
JsonSchema getJsonSchema()
{
    return JsonReflector<T>::getSchema();
}

template<typename T>
JsonValue serializeToJson(const T &value)
{
    return JsonReflector<T>::serialize(value);
}

template<typename T>
T deserializeJson(const JsonValue &json)
{
    return JsonReflector<T>::deserialize(json);
}

template<typename T>
std::string stringifyToJson(const T &value)
{
    auto json = serializeToJson(value);
    return stringifyToJson(json);
}

template<typename T>
T parseJson(const std::string &data)
{
    auto json = parseJson(data);
    return deserializeJson<T>(json);
}
}
