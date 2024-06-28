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
#include <future>
#include <string>
#include <type_traits>
#include <utility>

#include <brayns/core/json/Json.h>
#include <brayns/core/utils/FunctorReflector.h>

#include "Api.h"
#include "ApiReflector.h"
#include "Endpoint.h"
#include "Task.h"

namespace brayns
{
template<typename T>
using GetParamsType = std::decay_t<GetArgType<T, 0>>;

template<typename T>
using GetResultType = std::decay_t<GetReturnType<T>>;

template<ApiReflected T>
struct Task
{
    std::function<ProgressInfo()> getProgress;
    std::function<T()> wait;
    std::function<void()> cancel;
};

template<typename T>
struct TaskReflector;

template<ApiReflected T>
struct TaskReflector<Task<T>>
{
    using Result = T;
};

template<typename T>
concept ReflectedTask = requires { typename TaskReflector<T>::Result; };

template<ReflectedTask T>
using GetTaskResultType = typename TaskReflector<T>::Result;

template<typename T>
concept WithReflectedParams = getArgCount<T> == 1 && ApiReflected<GetParamsType<T>>;

template<typename T>
concept WithoutParams = getArgCount<T> == 0;

template<typename T>
concept WithReflectableParams = WithReflectedParams<T> || WithoutParams<T>;

template<typename T>
concept WithReflectedResult = ApiReflected<GetResultType<T>>;

template<typename T>
concept WithoutResult = std::is_void_v<GetResultType<T>>;

template<typename T>
concept WithReflectableResult = WithReflectedResult<T> || WithoutResult<T>;

template<typename T>
concept WithReflectedTaskResult = ApiReflected<GetTaskResultType<GetResultType<T>>>;

template<typename T>
concept WithoutTaskResult = std::is_void_v<GetTaskResultType<GetResultType<T>>>;

template<typename T>
concept WithReflectableTaskResult = WithReflectedTaskResult<T> || WithoutTaskResult<T>;

template<typename T>
concept ReflectedLauncher = WithReflectedParams<T> && WithReflectedTaskResult<T>;

template<typename T>
concept ReflectableLauncher = WithReflectableParams<T> && WithReflectableTaskResult<T>;

template<typename T>
concept ReflectedRunner = WithReflectedParams<T> && WithReflectedResult<T>;

template<typename T>
concept ReflectableRunner = WithReflectableParams<T> && WithReflectableResult<T>;

auto ensureHasParams(WithReflectedParams auto launcher)
{
    return launcher;
}

auto ensureHasParams(WithoutParams auto launcher)
{
    return [launcher = std::move(launcher)](NullJson) { return launcher(); };
}

auto ensureHasResult(WithReflectedResult auto launcher)
{
    return launcher;
}

auto ensureHasResult(WithoutResult auto launcher)
{
    using Params = GetParamsType<decltype(launcher)>;

    return [launcher = std::move(launcher)](Params params)
    {
        launcher(std::move(params));
        return NullJson();
    };
}

auto ensureHasTaskResult(WithReflectedTaskResult auto launcher)
{
    return launcher;
}

auto ensureHasTaskResult(WithoutTaskResult auto launcher)
{
    using Params = GetParamsType<decltype(launcher)>;

    return [launcher = std::move(launcher)](Params params)
    {
        auto task = launcher(std::move(params));

        auto waitAndReturnNull = [wait = std::move(task.wait)]
        {
            wait();
            return NullJson();
        };

        return Task<NullJson>{
            .getProgress = std::move(task.getProgress),
            .wait = std::move(waitAndReturnNull),
            .cancel = std::move(task.cancel),
        };
    };
}

template<ApiReflected T>
RawTask addParsingToTask(Task<T> task)
{
    using ResultReflector = ApiReflector<T>;

    return {
        .getProgress = std::move(task.getProgress),
        .wait = [wait = std::move(task.wait)] { return ResultReflector::serialize(wait()); },
        .cancel = std::move(task.cancel),
    };
}

template<ReflectedLauncher T>
TaskLauncher addParsingToTaskLauncher(T launcher)
{
    using ParamsReflector = ApiReflector<GetParamsType<T>>;

    return [launcher = std::move(launcher)](auto rawParams)
    {
        auto params = ParamsReflector::deserialize(std::move(rawParams));
        auto task = launcher(std::move(params));
        return addParsingToTask(std::move(task));
    };
}

template<ReflectedLauncher T>
EndpointSchema reflectAsyncEndpointSchema(std::string method)
{
    using ParamsReflector = ApiReflector<GetParamsType<T>>;
    using ResultReflector = ApiReflector<GetTaskResultType<GetResultType<T>>>;

    return {
        .method = std::move(method),
        .params = ParamsReflector::getSchema(),
        .result = ResultReflector::getSchema(),
        .async = true,
    };
}

template<ReflectedRunner T>
TaskRunner addParsingToTaskRunner(T handler)
{
    using ParamsReflector = ApiReflector<GetParamsType<T>>;
    using ResultReflector = ApiReflector<GetResultType<T>>;

    return [handler = std::move(handler)](auto rawParams)
    {
        auto params = ParamsReflector::deserialize(std::move(rawParams));
        auto result = handler(std::move(params));
        return ResultReflector::serialize(std::move(result));
    };
}

template<ReflectedRunner T>
EndpointSchema reflectEndpointSchema(std::string method)
{
    using ParamsReflector = ApiReflector<GetParamsType<T>>;
    using ResultReflector = ApiReflector<GetResultType<T>>;

    return {
        .method = std::move(method),
        .params = ParamsReflector::getSchema(),
        .result = ResultReflector::getSchema(),
    };
}

template<ApiReflected ParamsType, std::invocable<Progress, ParamsType> Handler>
Task<std::invoke_result_t<Handler, Progress, ParamsType>> startTask(Handler handler, ParamsType params)
{
    auto state = std::make_shared<ProgressState>();

    auto future = std::async(std::launch::async, std::move(handler), Progress(state), std::move(params));
    auto shared = std::make_shared<decltype(future)>(std::move(future));

    return {
        .getProgress = [=] { return state->get(); },
        .wait = [=] { return shared->get(); },
        .cancel = [=] { state->cancel(); },
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
    EndpointBuilder task(std::string method, ReflectableLauncher auto launcher)
    {
        auto reflected = ensureHasTaskResult(ensureHasParams(std::move(launcher)));
        auto schema = reflectAsyncEndpointSchema<decltype(reflected)>(std::move(method));
        auto startTask = addParsingToTaskLauncher(std::move(reflected));
        return add({std::move(schema), std::move(startTask)});
    }

    EndpointBuilder endpoint(std::string method, ReflectableRunner auto runner)
    {
        auto reflected = ensureHasResult(ensureHasParams(std::move(runner)));
        auto schema = reflectEndpointSchema<decltype(reflected)>(std::move(method));
        auto runTask = addParsingToTaskRunner(std::move(reflected));
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
