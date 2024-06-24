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

#include <string>
#include <type_traits>

#include <brayns/core/jsonrpc/Errors.h>
#include <brayns/core/jsonv2/Json.h>
#include <brayns/core/utils/FunctorReflector.h>

#include "Endpoint.h"

namespace brayns::experimental
{
template<typename T>
struct ApiReflector
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

template<typename T>
struct Params
{
    T value;
    std::string binary = {};
};

template<typename T>
struct ApiReflector<Params<T>>
{
    static JsonSchema getSchema()
    {
        return getJsonSchema<T>();
    }

    static Params<T> deserialize(RawParams params)
    {
        return {deserializeAs<T>(params.json), std::move(params.binary)};
    }
};

template<typename T>
struct Result
{
    T value;
    std::string binary = {};
};

template<typename T>
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
};

template<typename T>
using GetParamsType = std::decay_t<GetArgType<T, 0>>;

template<typename T>
using GetResultType = std::decay_t<GetReturnType<T>>;

template<typename HandlerType>
auto ensureHasParams(HandlerType handler)
    requires(getArgCount<HandlerType> == 1)
{
    return handler;
}

template<typename HandlerType>
auto ensureHasParams(HandlerType handler)
    requires(getArgCount<HandlerType> == 0)
{
    return [handler = std::move(handler)](NullJson) { return handler(); };
}

template<typename HandlerType>
auto ensureHasResult(HandlerType handler)
    requires(!std::is_void_v<GetResultType<HandlerType>>)
{
    return handler;
}

template<typename HandlerType>
auto ensureHasResult(HandlerType handler)
    requires(std::is_void_v<GetResultType<HandlerType>>)
{
    using Params = GetParamsType<HandlerType>;
    return [handler = std::move(handler)](Params params)
    {
        handler(std::move(params));
        return NullJson();
    };
}

template<typename HandlerType>
EndpointSchema reflectEndpointSchema(std::string method)
{
    using ParamsReflector = ApiReflector<GetParamsType<HandlerType>>;
    using ResultReflector = ApiReflector<GetResultType<HandlerType>>;

    return {
        .method = std::move(method),
        .params = ParamsReflector::getSchema(),
        .result = ResultReflector::getSchema(),
    };
}

template<typename HandlerType>
auto addEndpointParsing(HandlerType handler)
{
    using ParamsReflector = ApiReflector<GetParamsType<HandlerType>>;
    using ResultReflector = ApiReflector<GetResultType<HandlerType>>;

    return [handler = std::move(handler)](auto rawParams)
    {
        auto params = ParamsReflector::deserialize(std::move(rawParams));
        auto result = handler(std::move(params));
        return ResultReflector::serialize(std::move(result));
    };
}

class EndpointBuilder
{
public:
    explicit EndpointBuilder(Endpoint &endpoint):
        _endpoint(&endpoint)
    {
    }

    EndpointBuilder description(std::string value)
    {
        _endpoint->schema.description = std::move(value);
        return *this;
    }

private:
    Endpoint *_endpoint;
};

class ApiBuilder
{
public:
    EndpointRegistry build();

    EndpointBuilder endpoint(std::string method, auto handler)
    {
        auto wrapper = ensureHasResult(ensureHasParams(std::move(handler)));
        auto schema = reflectEndpointSchema<decltype(wrapper)>(std::move(method));
        auto run = addEndpointParsing(std::move(wrapper));
        _endpoints.push_back({std::move(schema), std::move(run)});
        return EndpointBuilder(_endpoints.back());
    }

private:
    std::vector<Endpoint> _endpoints;
};
}
