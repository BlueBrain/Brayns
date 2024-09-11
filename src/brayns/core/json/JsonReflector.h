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

#include <concepts>
#include <utility>

#include "JsonSchema.h"
#include "JsonValue.h"

namespace brayns
{

template<typename T>
struct JsonReflector;

template<typename T>
concept ReflectedJson = requires(T value, JsonValue json) {
    { JsonReflector<T>::getSchema() } -> std::same_as<JsonSchema>;
    { JsonReflector<T>::serialize(std::as_const(value), json) };
    { JsonReflector<T>::deserialize(std::as_const(json), value) };
};

template<ReflectedJson T>
JsonSchema getJsonSchema()
{
    return JsonReflector<T>::getSchema();
}

template<ReflectedJson T>
void serializeToJson(const T &value, JsonValue &json)
{
    return JsonReflector<T>::serialize(value, json);
}

template<ReflectedJson T>
JsonValue serializeToJson(const T &value)
{
    auto json = JsonValue();
    serializeToJson(value, json);
    return json;
}

template<ReflectedJson T>
void deserializeJson(const JsonValue &json, T &value)
{
    return JsonReflector<T>::deserialize(json, value);
}

template<ReflectedJson T>
T deserializeJsonAs(const JsonValue &json)
{
    auto value = T{};
    deserializeJson(json, value);
    return value;
}

template<ReflectedJson T>
std::string stringifyToJson(const T &value)
{
    auto json = serializeToJson(value);
    return stringify(json);
}

template<ReflectedJson T>
T parseJsonAs(const std::string &data)
{
    auto json = parseJson(data);
    return deserializeJsonAs<T>(json);
}
}
