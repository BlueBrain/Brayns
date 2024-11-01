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

#include "TaskManager.h"

#include <cassert>
#include <ranges>

namespace
{
using namespace brayns;

auto findTask(const std::deque<ManagedTask> &tasks, const JsonRpcId &id)
{
    return std::ranges::find_if(tasks, [&](const auto &task) { return task.id == id; });
}

const ManagedTask &getTask(const std::deque<ManagedTask> &tasks, const JsonRpcId &id)
{
    auto i = findTask(tasks, id);

    if (i != tasks.end())
    {
        return *i;
    }

    throw InvalidParams(fmt::format("No tasks found with ID {}", toString(id)));
}

void validateTaskId(const std::deque<ManagedTask> &tasks, const std::optional<JsonRpcId> &id)
{
    if (!id)
    {
        return;
    }

    auto i = findTask(tasks, *id);

    if (i != tasks.end())
    {
        throw InvalidParams(fmt::format("A task with ID {} is already registered", toString(*id)));
    }
}
}

namespace brayns
{
std::vector<JsonRpcId> TaskQueue::getAll()
{
    auto lock = std::lock_guard(_mutex);

    auto result = std::vector<JsonRpcId>();
    result.reserve(_tasks.size());

    for (const auto &task : _tasks)
    {
        if (task.id)
        {
            result.push_back(*task.id);
        }
    }

    return result;
}

TaskOperation TaskQueue::getCurrentOperation(const JsonRpcId &id)
{
    auto lock = std::lock_guard(_mutex);

    const auto &task = getTask(_tasks, id);
    return task.getCurrentOperation();
}

void TaskQueue::cancel(const JsonRpcId &id)
{
    auto lock = std::lock_guard(_mutex);

    const auto &task = getTask(_tasks, id);
    task.cancel();
}

void TaskQueue::add(ManagedTask task)
{
    auto lock = std::lock_guard(_mutex);

    validateTaskId(_tasks, task.id);

    _tasks.push_back(std::move(task));
}

void TaskQueue::runNext()
{
    assert(!_tasks.empty());

    auto &task = _tasks.front();
    task.run();

    auto lock = std::lock_guard(_mutex);
    _tasks.pop_front();
}

TaskManager::TaskManager(std::unique_ptr<TaskQueue> tasks, Worker worker):
    _tasks(std::move(tasks)),
    _worker(std::move(worker))
{
}

std::vector<JsonRpcId> TaskManager::getAll()
{
    return _tasks->getAll();
}

void TaskManager::add(ManagedTask task)
{
    if (task.priority)
    {
        task.run();
        return;
    }

    _tasks->add(std::move(task));
    _worker.submit([&tasks = *_tasks] { tasks.runNext(); });
}

TaskOperation TaskManager::getCurrentOperation(const JsonRpcId &id)
{
    return _tasks->getCurrentOperation(id);
}

void TaskManager::cancel(const JsonRpcId &id)
{
    _tasks->cancel(id);
}

TaskManager createTaskManager()
{
    auto tasks = std::make_unique<TaskQueue>();
    auto worker = startWorker();

    return TaskManager(std::move(tasks), std::move(worker));
}
}
