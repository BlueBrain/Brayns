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

    if (_runningTask && _runningTask->id == id)
    {
        return _runningTask->monitor.getCurrentOperation();
    }

    auto i = findTask(_tasks, id);

    if (i == _tasks.end())
    {
        throw InvalidParams(fmt::format("No tasks found with ID {}", toString(id)));
    }

    return notStartedYet();
}

void TaskQueue::cancel(const JsonRpcId &id)
{
    auto lock = std::lock_guard(_mutex);

    if (_runningTask && _runningTask->id == id)
    {
        return _runningTask->monitor.cancel();
    }

    auto i = findTask(_tasks, id);

    if (i == _tasks.end())
    {
        throw InvalidParams(fmt::format("No tasks found with ID {}", toString(id)));
    }

    i->cancel();
    _tasks.erase(i);
}

void TaskQueue::add(ManagedTask task)
{
    auto lock = std::lock_guard(_mutex);

    if (task.id && findTask(_tasks, *task.id) != _tasks.end())
    {
        throw InvalidParams(fmt::format("A task with ID {} is already in queue", toString(*task.id)));
    }

    _tasks.push_back(std::move(task));
}

void TaskQueue::runNext()
{
    {
        auto lock = std::lock_guard(_mutex);
        assert(!_runningTask);
        assert(!_tasks.empty());

        auto task = std::move(_tasks.front());
        _tasks.pop_front();

        auto monitor = task.start();

        if (!monitor)
        {
            return;
        }

        _runningTask = RunningTask{std::move(task.id), std::move(*monitor)};
    }

    _runningTask->monitor.wait();

    auto lock = std::lock_guard(_mutex);
    _runningTask.reset();
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
        auto monitor = task.start();

        if (monitor)
        {
            monitor->wait();
        }

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
