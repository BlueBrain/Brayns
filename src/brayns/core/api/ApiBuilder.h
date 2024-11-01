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
concept WithReflectableParams = WithReflectedParams<T> || WithoutParams<T>;

template<typename T>
concept WithReflectedResult = ReflectedPayload<ResultOf<T>>;

template<typename T>
concept WithoutResult = std::is_void_v<ResultOf<T>>;

template<typename T>
concept WithReflectableResult = WithReflectedResult<T> || WithoutResult<T>;

template<typename T>
concept WithReflectedTaskParams = ReflectedPayload<TaskParamsOf<ResultOf<T>>>;

template<typename T>
concept WithReflectedTaskResult = ReflectedPayload<TaskResultOf<ResultOf<T>>>;

template<typename T>
concept ReflectableTaskRunner = WithReflectableParams<T> && WithReflectableResult<T>;

template<typename T>
concept TaskRunner = WithReflectedParams<T> && WithReflectedResult<T>;

template<typename T>
concept TaskFactory = WithReflectedTaskParams<T> && WithReflectedTaskResult<T>;

auto ensureHasParams(WithReflectedParams auto run)
{
    return run;
}

auto ensureHasParams(WithoutParams auto run)
{
    return [run = std::move(run)](NullJson) { return run(); };
}

auto ensureHasResult(WithReflectedResult auto run)
{
    return run;
}

auto ensureHasResult(WithoutResult auto run)
{
    return [run = std::move(run)](ParamsOf<decltype(run)> params)
    {
        run(std::move(params));
        return NullJson();
    };
}

template<TaskRunner T>
auto wrapAsTaskFactory(T run)
{
    return [run = std::move(run)] { return createTaskWithoutProgress<ParamsOf<T>, ResultOf<T>>(run); };
}

template<TaskFactory T>
auto addParsingToTaskFactory(T start)
{
    return [start = std::move(start)] { return addParsingToTask(start()); };
}

template<TaskFactory T>
EndpointSchema reflectEndpointSchema(std::string method)
{
    return {
        .method = std::move(method),
        .params = PayloadReflector<TaskParamsOf<ResultOf<T>>>::getSchema(),
        .result = PayloadReflector<TaskResultOf<ResultOf<T>>>::getSchema(),
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

    EndpointBuilder priority(bool value)
    {
        _endpoint->priority = value;
        return *this;
    }

private:
    Endpoint *_endpoint;
};

class ApiBuilder
{
public:
    explicit ApiBuilder(EndpointRegistry &endpoints):
        _endpoints(&endpoints)
    {
    }

    EndpointBuilder endpoint(std::string method, ReflectableTaskRunner auto run)
    {
        auto withParams = ensureHasParams(std::move(run));
        auto withResult = ensureHasResult(std::move(withParams));
        auto start = wrapAsTaskFactory(std::move(withResult));
        return task(std::move(method), std::move(start));
    }

    EndpointBuilder task(std::string method, TaskFactory auto start)
    {
        auto schema = reflectEndpointSchema<decltype(start)>(std::move(method));
        auto withParsing = addParsingToTaskFactory(std::move(start));
        auto &current = _endpoints->add(Endpoint{std::move(schema), std::move(withParsing)});
        return EndpointBuilder(current);
    }

private:
    EndpointRegistry *_endpoints;
};
}
