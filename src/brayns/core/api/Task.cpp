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

#include "Task.h"

#include <cassert>

#include <fmt/format.h>

namespace
{
using namespace brayns;

const RawTask &getTask(const std::map<TaskId, RawTask> &tasks, TaskId id)
{
    auto i = tasks.find(id);

    if (i == tasks.end())
    {
        throw InvalidParams(fmt::format("Invalid task ID: {}", id));
    }

    return i->second;
}
}

namespace brayns
{
TaskManager::~TaskManager()
{
    for (const auto &[id, task] : _tasks)
    {
        task.cancel();
    }
}

std::vector<TaskInfo> TaskManager::getTasks() const
{
    auto infos = std::vector<TaskInfo>();
    infos.reserve(_tasks.size());

    for (const auto &[id, task] : _tasks)
    {
        infos.push_back({id, task.getProgress()});
    }

    return infos;
}

TaskId TaskManager::add(RawTask task)
{
    auto id = _ids.next();
    assert(!_tasks.contains(id));

    try
    {
        _tasks[id] = std::move(task);
    }
    catch (...)
    {
        _ids.recycle(id);
        throw;
    }

    return id;
}

ProgressInfo TaskManager::getProgress(TaskId id) const
{
    const auto &task = getTask(_tasks, id);
    return task.getProgress();
}

RawResult TaskManager::wait(TaskId id)
{
    const auto &task = getTask(_tasks, id);
    auto result = task.wait();
    _tasks.erase(id);
    _ids.recycle(id);
    return result;
}

void TaskManager::cancel(TaskId id)
{
    const auto &task = getTask(_tasks, id);
    task.cancel();
    _tasks.erase(id);
    _ids.recycle(id);
}
}
