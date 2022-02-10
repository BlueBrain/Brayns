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
#include <stdexcept>

namespace brayns
{
void NetworkTaskManager::add(const ClientRef &client, const RequestId &id, std::unique_ptr<NetworkTask> task)
{
    assert(task);
    auto &oldTask = _tasks[client][id];
    if (oldTask)
    {
        throw std::invalid_argument("A task is already registered with this client and request ID");
    }
    oldTask = std::move(task);
    oldTask->start();
}

void NetworkTaskManager::cancel(const ClientRef &client, const RequestId &id)
{
    auto i = _tasks.find(client);
    if (i == _tasks.end())
    {
        throw std::invalid_argument("No task running for this client");
    }
    auto &tasks = i->second;
    auto j = tasks.find(id);
    if (j == tasks.end())
    {
        throw std::invalid_argument("No task running with this request ID");
    }
    auto &task = j->second;
    task->cancel();
}

void NetworkTaskManager::disconnect(const ClientRef &client)
{
    auto i = _tasks.find(client);
    if (i == _tasks.end())
    {
        return;
    }
    auto &tasks = i->second;
    for (const auto &[id, task] : tasks)
    {
        task->disconnect();
    }
}

void NetworkTaskManager::poll()
{
    for (auto i = _tasks.begin(); i != _tasks.end();)
    {
        auto &tasks = i->second;
        for (auto j = tasks.begin(); j != tasks.end();)
        {
            auto &task = j->second;
            if (task->poll())
            {
                j = tasks.erase(j);
                continue;
            }
            ++j;
        }
        if (tasks.empty())
        {
            i = _tasks.erase(i);
            continue;
        }
        ++i;
    }
}
} // namespace brayns
