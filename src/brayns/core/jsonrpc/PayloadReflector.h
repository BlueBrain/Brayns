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
#include <stdexcept>
#include <string>

#include <brayns/core/json/Json.h>
#include <brayns/core/jsonrpc/Errors.h>
#include <brayns/core/jsonrpc/Messages.h>

namespace brayns
{
template<typename T>
struct PayloadReflector;

template<typename T>
concept ReflectedPayload = requires(T value) {
    { PayloadReflector<T>::getSchema() } -> std::same_as<JsonSchema>;
    { PayloadReflector<T>::serialize(std::move(value)) } -> std::same_as<Payload>;
    { PayloadReflector<T>::deserialize(Payload()) } -> std::same_as<T>;
};

template<>
struct PayloadReflector<Payload>
{
    static JsonSchema getSchema()
    {
        return getJsonSchema<JsonValue>();
    }

    static Payload serialize(Payload result)
    {
        return result;
    }

    static Payload deserialize(Payload params)
    {
        return params;
    }
};

template<ReflectedJson T>
struct PayloadReflector<T>
{
    static JsonSchema getSchema()
    {
        return getJsonSchema<T>();
    }

    static Payload serialize(T result)
    {
        return {serializeToJson(result)};
    }

    static T deserialize(Payload params)
    {
        if (!params.binary.empty())
        {
            throw InvalidParams("This endpoint does not accept additional binary data");
        }

        return deserializeJsonAs<T>(params.json);
    }
};

template<ReflectedJson T>
struct Params
{
    T value;
    std::vector<char> binary = {};
};

template<ReflectedJson T>
using Result = Params<T>;

template<ReflectedJson T>
struct PayloadReflector<Params<T>>
{
    static JsonSchema getSchema()
    {
        return getJsonSchema<T>();
    }

    static Payload serialize(Params<T> result)
    {
        return {serializeToJson(result.value), std::move(result.binary)};
    }

    static Params<T> deserialize(Payload params)
    {
        return {deserializeJsonAs<T>(params.json), std::move(params.binary)};
    }
};
}
