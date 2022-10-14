/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/utils/Log.h>

#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace brayns
{
void TaskManager::add(std::unique_ptr<ITask> task)
{
    auto &method = task->getMethod();
    if (task->hasPriority())
    {
        Log::debug("Task '{}' has priority and is run directly.", method);
        task->run();
        return;
    }
    Log::debug("Task '{}' queued for execution.", method);
    _tasks.push_back(std::move(task));
}

void TaskManager::runAllTasks()
{
    while (!_tasks.empty())
    {
        auto count = _tasks.size();
        Log::debug("Flushing task queue: {} remaining.", count);
        auto &task = *_tasks.front();
        task.run();
        _tasks.pop_front();
    }
}

void TaskManager::disconnect(const ClientRef &client)
{
    Log::debug("Cancelling all tasks from client {}.", client);
    for (const auto &task : _tasks)
    {
        if (task->getClient() != client)
        {
            continue;
        }
        task->disconnect();
    }
}

void TaskManager::cancel(const ClientRef &client, const RequestId &id)
{
    if (id.isEmpty())
    {
        throw InvalidParamsException("Empty task ID");
    }
    Log::debug("Cancelling task from client {} with id {}.", client, id);
    bool found = false;
    for (const auto &task : _tasks)
    {
        if (task->getClient() != client)
        {
            continue;
        }
        if (task->getId() != id)
        {
            continue;
        }
        found = true;
        task->cancel();
    }
    if (!found)
    {
        auto message = fmt::format("No requests found with client {} and ID {}.", client, id);
        throw InvalidParamsException(message);
    }
}
} // namespace brayns
