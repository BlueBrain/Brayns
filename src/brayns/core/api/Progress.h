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

#include <memory>
#include <mutex>
#include <string>

#include <brayns/core/json/Json.h>
#include <brayns/core/jsonrpc/Errors.h>

namespace brayns
{
struct TaskOperation
{
    std::string description = "Task startup";
    float completion = 0.0F;
    std::size_t index = 0;
};

template<>
struct JsonObjectReflector<TaskOperation>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<TaskOperation>();
        builder.field("description", [](auto &object) { return &object.description; })
            .description("Operation description");
        builder.field("completion", [](auto &object) { return &object.completion; })
            .description("Operation completion between 0 and 1");
        builder.field("index", [](auto &object) { return &object.index; })
            .description("Operation index between 0 and operation_count");
        return builder.build();
    }
};

class TaskCancelledException : public JsonRpcException
{
public:
    explicit TaskCancelledException();
};

class TaskMonitor
{
public:
    explicit TaskMonitor(std::size_t operationCount);

    std::size_t getOperationCount() const;
    TaskOperation getCurrentOperation();
    void update(float completion);
    void nextOperation(std::string description);
    void cancel();

private:
    std::mutex _mutex;
    std::size_t _operationCount;
    TaskOperation _currentOperation;
    bool _cancelled = false;
};

class Progress
{
public:
    explicit Progress(std::shared_ptr<TaskMonitor> monitor);

    void update(float completion);
    void nextOperation(std::string description);

private:
    std::shared_ptr<TaskMonitor> _monitor;
};
}
