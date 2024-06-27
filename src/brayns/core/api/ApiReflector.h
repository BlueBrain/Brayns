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
#include <type_traits>

#include <brayns/core/json/Json.h>
#include <brayns/core/jsonrpc/Errors.h>
#include <brayns/core/utils/FunctorReflector.h>

namespace brayns::experimental
{
struct RawParams
{
    JsonValue json;
    std::string binary = {};
};

struct RawResult
{
    JsonValue json;
    std::string binary = {};
};

template<typename T>
struct ApiReflector;

template<typename T>
concept ApiReflected = std::same_as<JsonSchema, decltype(ApiReflector<T>::getSchema())>
    && std::same_as<RawResult, decltype(ApiReflector<T>::serialize(std::declval<T>()))>
    && std::same_as<T, decltype(ApiReflector<T>::deserialize(RawParams()))>;

template<ReflectedJson T>
struct ApiReflector<T>
{
    static JsonSchema getSchema()
    {
        return getJsonSchema<T>();
    }

    static RawResult serialize(T value)
    {
        return {serializeToJson(value)};
    }

    static T deserialize(RawParams params)
    {
        if (!params.binary.empty())
        {
            throw InvalidParams("This endpoint does not accept additional binary data");
        }

        return deserializeAs<T>(params.json);
    }
};

template<ReflectedJson T>
struct Params
{
    T value;
    std::string binary = {};
};

template<ReflectedJson T>
struct ApiReflector<Params<T>>
{
    static JsonSchema getSchema()
    {
        return getJsonSchema<T>();
    }

    static RawResult serialize(Params<T> value)
    {
        throw std::invalid_argument("Cannot serialize params");
    }

    static Params<T> deserialize(RawParams params)
    {
        return {deserializeAs<T>(params.json), std::move(params.binary)};
    }
};

template<ReflectedJson T>
struct Result
{
    T value;
    std::string binary = {};
};

template<ReflectedJson T>
struct ApiReflector<Result<T>>
{
    static JsonSchema getSchema()
    {
        return getJsonSchema<T>();
    }

    static RawResult serialize(Result<T> result)
    {
        return {serializeToJson(result.value), std::move(result.binary)};
    }

    static Result<T> deserialize(RawParams params)
    {
        throw std::invalid_argument("Cannot deserialize result");
    }
};

template<typename T>
using GetParamsType = std::decay_t<GetArgType<T, 0>>;

template<typename T>
using GetResultType = std::decay_t<GetReturnType<T>>;
}
