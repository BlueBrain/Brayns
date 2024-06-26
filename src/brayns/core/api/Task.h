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
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <brayns/core/jsonv2/Json.h>
#include <brayns/core/utils/IdGenerator.h>

#include "ApiReflector.h"
#include "Progress.h"

namespace brayns::experimental
{
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
        builder.field("task_id", [](auto &object) { return &object.taskId; });
        return builder.build();
    }
};

struct RawTask
{
    std::function<ProgressInfo()> getProgress;
    std::function<RawResult()> wait;
    std::function<void()> cancel;
};

struct TaskInfo
{
    TaskId id;
    ProgressInfo progress;
};

class TaskManager
{
public:
    std::vector<TaskInfo> getTasks() const;
    TaskId add(RawTask task);
    ProgressInfo getProgress(TaskId id) const;
    RawResult wait(TaskId id);
    void cancel(TaskId id);

private:
    std::map<TaskId, RawTask> _tasks;
    IdGenerator<TaskId> _ids;
};

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
concept TaskLauncher =
    getArgCount<T> == 1 && ApiReflected<GetParamsType<T>> && ApiReflected<GetTaskResultType<GetResultType<T>>>;

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

template<TaskLauncher T>
auto addParsingToTaskLauncher(T launcher)
{
    using ParamsReflector = ApiReflector<GetParamsType<T>>;

    return [launcher = std::move(launcher)](auto rawParams)
    {
        auto params = ParamsReflector::deserialize(std::move(rawParams));

        auto task = launcher(std::move(params));

        return addParsingToTask(std::move(task));
    };
}

template<typename T>
concept TaskHandler = getArgCount<T> == 1 && ApiReflected<GetParamsType<T>> && ApiReflected<GetResultType<T>>;

template<ApiReflected T>
Task<T> completedTask(T result)
{
    auto shared = std::make_shared<T>(std::move(result));
    auto done = ProgressInfo{{}, 1.0F};

    return {
        .getProgress = [=] { return done; },
        .wait = [=] { return std::move(*shared); },
        .cancel = [] { throw std::invalid_argument("Cannot cancel a completed task"); },
    };
}

template<TaskHandler T>
auto addParsingToTaskHandler(T handler)
{
    using ParamsReflector = ApiReflector<GetParamsType<T>>;

    return [handler = std::move(handler)](auto rawParams)
    {
        auto params = ParamsReflector::deserialize(std::move(rawParams));

        auto result = handler(std::move(params));

        auto task = completedTask(std::move(result));

        return addParsingToTask(std::move(task));
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
}
