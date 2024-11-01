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

#include <brayns/core/json/Json.h>
#include <brayns/core/jsonrpc/Messages.h>
#include <brayns/core/jsonrpc/PayloadReflector.h>

#include "Progress.h"

namespace brayns
{
struct Task
{
    std::function<Payload(Payload)> run;
    std::function<TaskOperation()> getCurrentOperation;
    std::function<void()> cancel;
};

template<ReflectedPayload Params, ReflectedPayload Result>
struct TaskOf
{
    std::function<Result(Params)> run;
    std::function<TaskOperation()> getCurrentOperation;
    std::function<void()> cancel;
};

template<ReflectedPayload Params, ReflectedPayload Result>
Task addParsingToTask(TaskOf<Params, Result> task)
{
    auto run = [run = std::move(task.run)](auto payload)
    {
        auto params = PayloadReflector<Params>::deserialize(std::move(payload));
        auto result = run(std::move(params));
        return PayloadReflector<Result>::serialize(std::move(result));
    };

    return {
        .run = std::move(run),
        .getCurrentOperation = std::move(task.getCurrentOperation),
        .cancel = std::move(task.cancel),
    };
}

template<ReflectedPayload Params, ReflectedPayload Result>
TaskOf<Params, Result> createTaskWithoutProgress(std::function<Result(Params)> run)
{
    return {
        .run = std::move(run),
        .getCurrentOperation = [] { return notStartedYet(); },
        .cancel = [] {},
    };
}

template<typename T>
struct TaskReflector
{
};

template<ReflectedPayload T, ReflectedPayload U>
struct TaskReflector<TaskOf<T, U>>
{
    using Params = T;
    using Result = U;
};

template<typename T>
using TaskParamsOf = typename TaskReflector<T>::Params;

template<typename T>
using TaskResultOf = typename TaskReflector<T>::Result;
}
