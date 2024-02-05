/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "NetworkTaskMap.h"

namespace brayns
{
/**
 * @brief Task manager to store all running tasks started by clients.
 *
 * A task is started by a JSON-RPC request and is hence bound to a client and a
 * request ID. These two objects can be used to retreive and monitor a task.
 *
 */
class NetworkTaskManager
{
public:
    /**
     * @brief Add a new task and cancel the old one.
     *
     * @param handle Client requesting the task.
     * @param id Task request ID from JSON-RPC.
     * @param task Task to register.
     */
    void addOrReplace(const ConnectionHandle& handle, const RequestId& id,
                      NetworkTaskPtr task)
    {
        auto oldTask = _tasks.find(handle, id);
        if (oldTask)
        {
            oldTask->cancelAndWait();
        }
        _tasks.add(handle, id, std::move(task));
    }

    /**
     * @brief Add a task if not already running.
     *
     * @param handle Client requesting the task.
     * @param id Task request ID from JSON-RPC.
     * @param task Task to register.
     * @return true No task with the same client / ID exists.
     * @return false Cannot register the task because already running.
     */
    bool addIfNotPresent(const ConnectionHandle& handle, const RequestId& id,
                         NetworkTaskPtr task)
    {
        auto oldTask = _tasks.find(handle, id);
        if (oldTask)
        {
            return false;
        }
        _tasks.add(handle, id, std::move(task));
        return true;
    }

    /**
     * @brief Notify all task bound to a client when this one disconnects.
     *
     * @param handle Client handle.
     */
    void disconnect(const ConnectionHandle& handle) const
    {
        _tasks.forEach(handle, [&](auto&, auto& task) { task.onDisconnect(); });
    }

    /**
     * @brief Cancel a task from its client and ID.
     *
     * @param handle Client connection handle.
     * @param id Task request ID.
     * @return true Task found and cancelled.
     * @return false Task not found.
     */
    bool cancel(const ConnectionHandle& handle, const RequestId& id) const
    {
        auto task = _tasks.find(handle, id);
        if (!task)
        {
            return false;
        }
        task->cancel();
        return true;
    }

    /**
     * @brief Poll all registered task.
     *
     * Polling a task checks if it is done, run cleanup if any and remove it
     * from the list.
     *
     */
    void poll()
    {
        _tasks.removeIf([](auto&, auto&, auto& task) {
            bool complete = !task.isRunning();
            task.poll();
            return complete;
        });
    }

private:
    NetworkTaskMap _tasks;
};
} // namespace brayns