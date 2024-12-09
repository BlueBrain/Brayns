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
#include <mutex>
#include <optional>

#include <brayns/core/json/Json.h>
#include <brayns/core/jsonrpc/Messages.h>
#include <brayns/core/utils/Threading.h>

#include "Progress.h"

namespace brayns
{
struct TaskMonitor
{
    std::function<void()> wait;
    std::function<TaskOperation()> getCurrentOperation;
    std::function<void()> cancel;
};

struct ManagedTask
{
    std::optional<JsonRpcId> id;
    bool priority;
    std::function<std::optional<TaskMonitor>()> start;
    std::function<void()> cancel;
};

struct RunningTask
{
    std::optional<JsonRpcId> id;
    TaskMonitor monitor;
};

class TaskQueue
{
public:
    std::vector<JsonRpcId> getAll();
    TaskOperation getCurrentOperation(const JsonRpcId &id);
    void cancel(const JsonRpcId &id);
    void add(ManagedTask task);
    void runNext();

private:
    std::mutex _mutex;
    std::optional<RunningTask> _runningTask;
    std::deque<ManagedTask> _tasks;
};

class TaskManager
{
public:
    explicit TaskManager(std::unique_ptr<TaskQueue> tasks, Worker worker);

    std::vector<JsonRpcId> getAll();
    void add(ManagedTask task);
    TaskOperation getCurrentOperation(const JsonRpcId &id);
    void cancel(const JsonRpcId &id);

private:
    std::unique_ptr<TaskQueue> _tasks;
    Worker _worker;
};

TaskManager createTaskManager();
}
