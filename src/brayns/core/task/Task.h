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
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <brayns/core/api/Endpoint.h>
#include <brayns/core/jsonrpc/Errors.h>
#include <brayns/core/jsonv2/Json.h>
#include <brayns/core/utils/IdGenerator.h>

#include "Progress.h"

namespace brayns::experimental
{
using TaskId = std::size_t;

struct TaskSchema
{
    std::string name;
    std::string description;
    JsonSchema params;
    JsonSchema result;
};

template<>
struct JsonReflector<TaskSchema>
{
    static auto reflect()
    {
        auto builder = JsonObjectInfoBuilder<TaskSchema>();
        builder.field("params", [](auto &object) { return &object.params; }).description("Task params schema");
        builder.field("result", [](auto &object) { return &object.result; }).description("Task result schema");
        return builder.build();
    }
};

struct RawTask
{
    std::function<ProgressInfo()> getProgress;
    std::function<RawResult()> wait;
    std::function<void()> cancel;
};

using TaskFactory = std::function<RawTask(RawParams)>;

struct TaskDefinition
{
    TaskSchema schema;
    TaskFactory start;
};

class TaskManager
{
public:
    explicit TaskManager(std::unordered_map<std::string, TaskDefinition> definitions);

    TaskId start(const std::string &name, RawParams params);
    ProgressInfo getProgress(TaskId id);
    RawResult wait(TaskId id);
    void cancel(TaskId id);

private:
    std::unordered_map<std::string, TaskDefinition> _definitions;
    std::unordered_map<TaskId, RawTask> _tasks;
    IdGenerator<TaskId> _ids;
};
}
