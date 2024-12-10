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
    std::function<Payload()> wait;
    std::function<TaskOperation()> getCurrentOperation;
    std::function<void()> cancel;
};

template<ReflectedPayload T>
struct TaskOf
{
    std::function<T()> wait;
    std::function<TaskOperation()> getCurrentOperation;
    std::function<void()> cancel;
};

template<ReflectedPayload T>
Task addParsingToTask(TaskOf<T> task)
{
    return {
        .wait = [wait = std::move(task.wait)] { return PayloadReflector<T>::serialize(wait()); },
        .getCurrentOperation = std::move(task.getCurrentOperation),
        .cancel = std::move(task.cancel),
    };
}

template<ReflectedPayload T>
TaskOf<T> completedTask(T result)
{
    auto ptr = std::make_shared<T>(std::move(result));

    return {
        .wait = [=] { return std::move(*ptr); },
        .getCurrentOperation = [] { return notStartedYet(); },
        .cancel = [] {},
    };
}

template<typename T>
struct TaskReflector
{
};

template<ReflectedPayload T>
struct TaskReflector<TaskOf<T>>
{
    using Result = T;
};

template<typename T>
using TaskResultOf = typename TaskReflector<T>::Result;
}
