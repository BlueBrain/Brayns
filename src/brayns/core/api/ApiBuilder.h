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
concept WithReflectedParams = getArgCount<T> == 1 && ReflectedPayload<ParamsOf<T>>;

template<typename T>
concept WithoutParams = getArgCount<T> == 0;

template<typename T>
concept WithParams = WithReflectedParams<T> || WithoutParams<T>;

template<typename T>
concept WithReflectedResult = ReflectedPayload<ResultOf<T>>;

template<typename T>
concept WithoutResult = std::is_void_v<ResultOf<T>>;

template<typename T>
concept WithResult = WithReflectedResult<T> || WithoutResult<T>;

template<typename T>
concept WithReflectedTaskResult = ReflectedPayload<GetTaskResult<ResultOf<T>>>;

template<typename T>
concept WithoutTaskResult = std::is_void_v<GetTaskResult<ResultOf<T>>>;

template<typename T>
concept WithTaskResult = WithReflectedTaskResult<T> || WithoutTaskResult<T>;

template<typename T>
concept ReflectedAsyncHandler = WithReflectedParams<T> && WithReflectedTaskResult<T>;

template<typename T>
concept AsyncHandler = WithParams<T> && WithTaskResult<T>;

template<typename T>
concept ReflectedSyncHandler = WithReflectedParams<T> && WithReflectedResult<T>;

template<typename T>
concept SyncHandler = WithParams<T> && WithResult<T>;

auto ensureHasParams(WithReflectedParams auto handler)
{
    return handler;
}

auto ensureHasParams(WithoutParams auto handler)
{
    return [handler = std::move(handler)](NullJson) { return handler(); };
}

auto ensureHasResult(WithReflectedResult auto handler)
{
    return handler;
}

auto ensureHasResult(WithoutResult auto handler)
{
    using Params = ParamsOf<decltype(handler)>;

    return [handler = std::move(handler)](Params params)
    {
        handler(std::move(params));
        return NullJson();
    };
}

auto ensureHasTaskResult(WithReflectedTaskResult auto handler)
{
    return handler;
}

auto ensureHasTaskResult(WithoutTaskResult auto handler)
{
    using Params = ParamsOf<decltype(handler)>;

    return [handler = std::move(handler)](Params params)
    {
        auto task = handler(std::move(params));

        auto waitAndReturnNull = [wait = std::move(task.wait)]
        {
            wait();
            return NullJson();
        };

        return Task<NullJson>{
            .operationCount = task.operationCount,
            .getCurrentOperation = std::move(task.getCurrentOperation),
            .wait = std::move(waitAndReturnNull),
            .cancel = std::move(task.cancel),
        };
    };
}

template<ReflectedPayload T>
TaskInterface addParsingToTask(Task<T> task)
{
    using ResultReflector = PayloadReflector<T>;

    return {
        .operationCount = task.operationCount,
        .getCurrentOperation = std::move(task.getCurrentOperation),
        .wait = [wait = std::move(task.wait)] { return ResultReflector::serialize(wait()); },
        .cancel = std::move(task.cancel),
    };
}

template<ReflectedAsyncHandler T>
AsyncEndpointHandler addParsingToAsyncHandler(T handler)
{
    using ParamsReflector = PayloadReflector<ParamsOf<T>>;

    return [handler = std::move(handler)](auto rawParams)
    {
        auto params = ParamsReflector::deserialize(std::move(rawParams));
        auto task = handler(std::move(params));
        return addParsingToTask(std::move(task));
    };
}

template<ReflectedAsyncHandler T>
EndpointSchema reflectAsyncEndpointSchema(std::string method)
{
    using ParamsReflector = PayloadReflector<ParamsOf<T>>;
    using ResultReflector = PayloadReflector<GetTaskResult<ResultOf<T>>>;

    return {
        .method = std::move(method),
        .params = ParamsReflector::getSchema(),
        .result = ResultReflector::getSchema(),
        .async = true,
    };
}

template<ReflectedSyncHandler T>
SyncEndpointHandler addParsingToSyncHandler(T handler)
{
    using ParamsReflector = PayloadReflector<ParamsOf<T>>;
    using ResultReflector = PayloadReflector<ResultOf<T>>;

    return [handler = std::move(handler)](auto rawParams)
    {
        auto params = ParamsReflector::deserialize(std::move(rawParams));
        auto result = handler(std::move(params));
        return ResultReflector::serialize(std::move(result));
    };
}

template<ReflectedSyncHandler T>
EndpointSchema reflectSyncEndpointSchema(std::string method)
{
    using ParamsReflector = PayloadReflector<ParamsOf<T>>;
    using ResultReflector = PayloadReflector<ResultOf<T>>;

    return {
        .method = std::move(method),
        .params = ParamsReflector::getSchema(),
        .result = ResultReflector::getSchema(),
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
    EndpointBuilder task(std::string method, AsyncHandler auto handler)
    {
        auto reflected = ensureHasTaskResult(ensureHasParams(std::move(handler)));
        auto schema = reflectAsyncEndpointSchema<decltype(reflected)>(std::move(method));
        auto startTask = addParsingToAsyncHandler(std::move(reflected));
        return add({std::move(schema), std::move(startTask)});
    }

    EndpointBuilder endpoint(std::string method, SyncHandler auto handler)
    {
        auto reflected = ensureHasResult(ensureHasParams(std::move(handler)));
        auto schema = reflectSyncEndpointSchema<decltype(reflected)>(std::move(method));
        auto runTask = addParsingToSyncHandler(std::move(reflected));
        return add({std::move(schema), std::move(runTask)});
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
