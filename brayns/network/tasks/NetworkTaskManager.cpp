/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "NetworkTaskManager.h"

#include <cassert>

namespace brayns
{
/**
 * @brief Add a task to the pool with the associated client and request ID.
 *
 * @param handle Connection handle of the client requesting the task.
 * @param id ID of the request starting the task.
 * @param task Abstract task (might be running or not).
 */
void NetworkTaskMap::add(const ConnectionHandle &handle, const RequestId &id, NetworkTaskPtr task)
{
    assert(task);
    _tasks[handle][id] = std::move(task);
}

/**
 * @brief Retreive a task using its client and request ID.
 *
 * @param handle Connection handle of the client requesting the task.
 * @param id ID of the request starting the task.
 * @return NetworkTask* Task or null if not found.
 */
NetworkTask *NetworkTaskMap::find(const ConnectionHandle &handle, const RequestId &id) const
{
    auto i = _tasks.find(handle);
    if (i == _tasks.end())
    {
        return nullptr;
    }
    auto &tasks = i->second;
    auto j = tasks.find(id);
    if (j == tasks.end())
    {
        return nullptr;
    }
    auto &task = j->second;
    return task.get();
}

void NetworkTaskManager::addOrReplace(const ConnectionHandle &handle, const RequestId &id, NetworkTaskPtr task)
{
    auto oldTask = _tasks.find(handle, id);
    if (oldTask)
    {
        oldTask->cancelAndWait();
    }
    _tasks.add(handle, id, std::move(task));
}

bool NetworkTaskManager::addIfNotPresent(const ConnectionHandle &handle, const RequestId &id, NetworkTaskPtr task)
{
    auto oldTask = _tasks.find(handle, id);
    if (oldTask)
    {
        return false;
    }
    _tasks.add(handle, id, std::move(task));
    return true;
}

void NetworkTaskManager::disconnect(const ConnectionHandle &handle) const
{
    _tasks.forEach(handle, [&](auto &, auto &task) { task.onDisconnect(); });
}

bool NetworkTaskManager::cancel(const ConnectionHandle &handle, const RequestId &id) const
{
    auto task = _tasks.find(handle, id);
    if (!task)
    {
        return false;
    }
    task->cancel();
    return true;
}

void NetworkTaskManager::poll()
{
    _tasks.removeIf(
        [](auto &, auto &, auto &task)
        {
            bool complete = !task.isRunning();
            task.poll();
            return complete;
        });
}
} // namespace brayns
