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

#include <algorithm>

#include <brayns/network/jsonrpc/JsonRpcException.h>

#include "BinaryTask.h"
#include "JsonRpcTask.h"

namespace
{
class TaskMatcher
{
public:
    static bool match(brayns::ITask &task, const brayns::ClientRef &client, const brayns::RequestId &id)
    {
        if (task.getClient() != client)
        {
            return false;
        }
        if (task.getId() != id)
        {
            return false;
        }
        return true;
    }
};

class TaskCanceller
{
public:
    static void cancel(
        std::deque<std::unique_ptr<brayns::ITask>> &tasks,
        const brayns::ClientRef &client,
        const brayns::RequestId &id)
    {
        auto first = tasks.begin();
        auto last = tasks.end();
        auto match = [&](auto &task) { return TaskMatcher::match(*task, client, id); };
        auto i = std::find_if(first, last, match);
        if (i == last)
        {
            throw brayns::TaskNotFoundException();
        }
        auto &task = **i;
        task.cancel();
    }
};
} // namespace

namespace brayns
{
void TaskManager::addBinaryTask(ClientRequest request, const EntrypointRegistry &entrypoints)
{
    auto task = std::make_unique<BinaryTask>(std::move(request), entrypoints);
    _tasks.push_back(std::move(task));
}

void TaskManager::addJsonRpcTask(JsonRpcRequest request, const EntrypointRef &entrypoint)
{
    auto task = std::make_unique<JsonRpcTask>(std::move(request), entrypoint);
    _tasks.push_back(std::move(task));
}

void TaskManager::runAllTasks()
{
    while (!_tasks.empty())
    {
        auto &task = *_tasks.front();
        task.run();
        _tasks.pop_front();
    }
}

void TaskManager::cancel(const ClientRef &client, const RequestId &id)
{
    TaskCanceller::cancel(_tasks, client, id);
}
} // namespace brayns
