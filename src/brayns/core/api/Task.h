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
#include <string>

#include <brayns/core/json/Json.h>

#include "Progress.h"

namespace brayns
{
struct RawParams
{
    JsonValue json;
    std::string binary = {};
};

struct RawResult
{
    JsonValue json;
    std::string binary = {};
};

struct RawTask
{
    std::function<ProgressInfo()> getProgress;
    std::function<RawResult()> wait;
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
        builder.field("task_id", [](auto &object) { return &object.taskId; });
        return builder.build();
    }
};

struct TaskInfo
{
    TaskId id;
    ProgressInfo progress;
};
}
