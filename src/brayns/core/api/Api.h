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

#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <type_traits>

#include <brayns/core/jsonrpc/Errors.h>
#include <brayns/core/jsonrpc/Messages.h>
#include <brayns/core/jsonv2/Json.h>
#include <brayns/core/utils/FunctorReflector.h>

namespace brayns::experimental
{
struct EndpointSchema
{
    std::string method;
    std::string description;
    JsonSchema params;
    JsonSchema result;
    bool binary_params = false;
    bool binary_result = false;
};

template<>
struct JsonReflector<EndpointSchema>
{
    static auto reflect()
    {
        auto builder = JsonObjectInfoBuilder<EndpointSchema>();
        builder.field("method", [](auto &object) { return &object.method; }).description("Endpoint method");
        builder.field("description", [](auto &object) { return &object.method; }).description("Endpoint description");
        builder.field("params", [](auto &object) { return &object.params; }).description("Endpoint params JSON schema");
        builder.field("result", [](auto &object) { return &object.result; }).description("Endpoint result JSON schema");
        builder.field("binary_params", [](auto &object) { return &object.binary_params; })
            .description("Wether endpoint accepts binary in addition of its JSON params");
        builder.field("binary_result", [](auto &object) { return &object.binary_result; })
            .description("Wether endpoint returns binary in addition of its JSON result");
        return builder.build();
    }
};

struct RawParams
{
    JsonValue json;
    std::string_view binary;
};

struct RawResult
{
    JsonValue json;
    std::string binary;
};

struct Endpoint
{
    EndpointSchema schema;
    std::function<RawResult(const RawParams &)> run;
};

class Api
{
public:
    explicit Api(std::map<std::string, Endpoint> endpoints);

    std::vector<std::string> getMethods() const;
    EndpointSchema getSchema(const std::string &method) const;
    JsonRpcResponse execute(const JsonRpcRequest &request);

private:
    std::map<std::string, Endpoint> _endpoints;
};

template<typename T>
struct Params
{
    T value;
    std::string_view binary;
};

template<typename T>
struct Result
{
    T value;
    std::string binary;
};

template<typename T>
struct ApiReflector
{
    static JsonSchema getSchema()
    {
        return getJsonSchema<T>();
    }

    static RawResult serialize(T value)
    {
        return {serializeToJson(value), {}};
    }

    static T deserialize(const RawParams &params)
    {
        return deserializeAs<T>(params.json);
    }

    static bool supportsBinary()
    {
        return false;
    }
};

template<typename T>
struct ApiReflector<Params<T>>
{
    static JsonSchema getSchema()
    {
        return getJsonSchema<T>();
    }

    static Params<T> deserialize(const RawParams &params)
    {
        return {deserializeAs<T>(params.json), params.binary};
    }

    static bool supportsBinary()
    {
        return true;
    }
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

    static bool supportsBinary()
    {
        return true;
    }
};

template<typename T>
using GetParamsType = std::decay_t<GetArgType<T, 0>>;

template<typename T>
using GetResultType = std::decay_t<GetReturnType<T>>;

template<typename T>
constexpr auto wrapEndpointHandler(T handler)
{
    using Result = GetResultType<T>;

    static_assert(getArgCount<T> <= 1, "Endpoint handlers can only have 0 or 1 params");

    constexpr auto hasParams = getArgCount<T> == 1;
    constexpr auto hasResult = !std::is_void_v<Result>;

    if constexpr (hasParams)
    {
        using Params = GetParamsType<T>;

        if constexpr (hasResult)
        {
            return [=](const Params &params) { return handler(params); };
        }
        else
        {
            return [=](const Params &params)
            {
                handler(params);
                return NullJson();
            };
        }
    }
    else
    {
        if constexpr (hasResult)
        {
            return [=](NullJson) { return handler(); };
        }
        else
        {
            return [=](NullJson)
            {
                handler();
                return NullJson();
            };
        }
    }
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
    Api build();

    template<typename CallableType>
    EndpointBuilder endpoint(std::string method, CallableType &&handler)
    {
        auto wrapper = wrapEndpointHandler(std::forward<CallableType>(handler));

        using Handler = decltype(wrapper);

        using Params = GetParamsType<Handler>;
        using Result = GetResultType<Handler>;

        using ParamsReflector = ApiReflector<Params>;
        using ResultReflector = ApiReflector<Result>;

        auto &endpoint = _endpoints.emplace_back();

        endpoint.schema.method = std::move(method);

        endpoint.schema.params = ParamsReflector::getSchema();
        endpoint.schema.binary_params = ParamsReflector::supportsBinary();

        endpoint.schema.result = ResultReflector::getSchema();
        endpoint.schema.binary_result = ResultReflector::supportsBinary();

        endpoint.run = [=](const auto &rawParams)
        {
            auto params = ParamsReflector::deserialize(rawParams);
            auto result = wrapper(params);
            return ResultReflector::serialize(std::move(result));
        };

        return EndpointBuilder(endpoint);
    }

private:
    std::vector<Endpoint> _endpoints;
};
}
