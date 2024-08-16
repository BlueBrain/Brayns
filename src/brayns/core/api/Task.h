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
#include <functional>
#include <future>
#include <string>
#include <type_traits>

#include <brayns/core/json/Json.h>
#include <brayns/core/jsonrpc/PayloadReflector.h>

#include "Progress.h"

namespace brayns
{
struct TaskInterface
{
    std::size_t operationCount;
    std::function<TaskOperation()> getCurrentOperation;
    std::function<Payload()> wait;
    std::function<void()> cancel;
};

using TaskId = std::uint32_t;

struct TaskResult
{
    TaskId taskId;
};

template<>
struct JsonObjectReflector<TaskResult>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<TaskResult>();
        builder.field("taskId", [](auto &object) { return &object.taskId; })
            .description("ID of the task started by the method");
        return builder.build();
    }
};

struct TaskInfo
{
    TaskId id;
    std::size_t operationCount;
    TaskOperation currentOperation;
};

template<>
struct JsonObjectReflector<TaskInfo>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<TaskInfo>();
        builder.field("id", [](auto &object) { return &object.id; }).description("Task ID");
        builder.field("operationCount", [](auto &object) { return &object.operationCount; })
            .description("Number of operations the task will perform");
        builder.field("currentOperation", [](auto &object) { return &object.currentOperation; })
            .description("Current task operation");
        return builder.build();
    }
};

template<ReflectedPayload T>
struct Task
{
    std::size_t operationCount;
    std::function<TaskOperation()> getCurrentOperation;
    std::function<T()> wait;
    std::function<void()> cancel;
};

template<typename T>
struct TaskReflector;

template<ReflectedPayload T>
struct TaskReflector<Task<T>>
{
    using Result = T;
};

template<typename T>
concept ReflectedTask = requires { typename TaskReflector<T>::Result; };

template<ReflectedTask T>
using GetTaskResult = typename TaskReflector<T>::Result;

template<ReflectedPayload ParamsType, std::invocable<Progress, ParamsType> Handler>
Task<std::invoke_result_t<Handler, Progress, ParamsType>> startTask(
    Handler handler,
    ParamsType params,
    std::size_t operationCount)
{
    auto monitor = std::make_shared<TaskMonitor>(operationCount);

    auto future = std::async(std::launch::async, std::move(handler), Progress(monitor), std::move(params));
    auto shared = std::make_shared<decltype(future)>(std::move(future));

    return {
        .operationCount = operationCount,
        .getCurrentOperation = [=] { return monitor->getCurrentOperation(); },
        .wait = [=] { return shared->get(); },
        .cancel = [=] { monitor->cancel(); },
    };
}
}
