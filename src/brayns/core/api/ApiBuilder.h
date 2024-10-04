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
#include <string>
#include <type_traits>
#include <utility>

#include <brayns/core/json/Json.h>
#include <brayns/core/jsonrpc/PayloadReflector.h>
#include <brayns/core/utils/FunctorReflector.h>

#include "Api.h"
#include "Endpoint.h"
#include "Task.h"

namespace brayns
{
template<typename T>
using ParamsOf = std::decay_t<GetArgType<T, 0>>;

template<typename T>
using ResultOf = std::decay_t<GetReturnType<T>>;

template<typename T>
concept WithReflectedParamsAndProgress = getArgCount<T> == 2 && ReflectedPayload<ParamsOf<T>> && std::same_as<Progress, GetArgType<T, 1>>;

template<typename T>
concept WithProgress = getArgCount<T> == 1 && std::same_as<Progress, GetArgType<T, 0>>;

template<typename T>
concept WithReflectedParams = getArgCount<T> == 1 && ReflectedPayload<ParamsOf<T>>;

template<typename T>
concept WithoutParams = getArgCount<T> == 0;

template<typename T>
concept WithParams = WithReflectedParamsAndProgress<T> || WithProgress<T> || WithReflectedParams<T> || WithoutParams<T>;

template<typename T>
concept WithReflectedResult = ReflectedPayload<ResultOf<T>>;

template<typename T>
concept WithoutResult = std::is_void_v<ResultOf<T>>;

template<typename T>
concept WithResult = WithReflectedResult<T> || WithoutResult<T>;

template<typename T>
concept ReflectableHandler = WithParams<T> && WithResult<T>;

template<typename T>
concept ReflectedHandler = WithReflectedParamsAndProgress<T> && WithReflectedResult<T>;

auto ensureHasParams(WithReflectedParamsAndProgress auto handler)
{
    return handler;
}

auto ensureHasParams(WithProgress auto handler)
{
    return [handler = std::move(handler)](NullJson, Progress progress) { return handler(progress); };
}

auto ensureHasParams(WithReflectedParams auto handler)
{
    return [handler = std::move(handler)](ParamsOf<decltype(handler)> params, Progress) { return handler(std::move(params)); };
}

auto ensureHasParams(WithoutParams auto handler)
{
    return [handler = std::move(handler)](NullJson, Progress) { return handler(); };
}

auto ensureHasResult(WithReflectedResult auto handler)
{
    return handler;
}

auto ensureHasResult(WithoutResult auto handler)
{
    return [handler = std::move(handler)](ParamsOf<decltype(handler)> params, Progress progress)
    {
        handler(std::move(params), progress);
        return NullJson();
    };
}

template<ReflectedHandler T>
EndpointHandler addParsingToHandler(T handler)
{
    return [handler = std::move(handler)](auto rawParams, auto progress)
    {
        auto params = PayloadReflector<ParamsOf<T>>::deserialize(std::move(rawParams));
        auto result = handler(std::move(params), progress);
        return PayloadReflector<ResultOf<T>>::serialize(std::move(result));
    };
}

template<ReflectedHandler T>
EndpointSchema reflectEndpointSchema(std::string method)
{
    return {
        .method = std::move(method),
        .params = PayloadReflector<ParamsOf<T>>::getSchema(),
        .result = PayloadReflector<ResultOf<T>>::getSchema(),
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
    EndpointBuilder endpoint(std::string method, ReflectableHandler auto handler)
    {
        auto reflected = ensureHasResult(ensureHasParams(std::move(handler)));
        auto schema = reflectEndpointSchema<decltype(reflected)>(std::move(method));
        auto run = addParsingToHandler(std::move(reflected));
        return add({std::move(schema), std::move(run)});
    }

    Api build()
    {
        return Api(std::exchange(_endpoints, {}));
    }

private:
    std::map<std::string, Endpoint> _endpoints;

    EndpointBuilder add(Endpoint endpoint)
    {
        const auto &method = endpoint.schema.method;

        if (_endpoints.contains(method))
        {
            throw std::invalid_argument("Duplicated endpoint method");
        }

        auto &emplaced = _endpoints[method] = std::move(endpoint);

        return EndpointBuilder(emplaced);
    }
};
}
