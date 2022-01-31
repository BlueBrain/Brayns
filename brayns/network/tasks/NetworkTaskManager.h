/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include <string>
#include <unordered_map>

#include <brayns/network/json/RequestId.h>
#include <brayns/network/socket/ConnectionHandle.h>

#include "NetworkTask.h"

namespace brayns
{
/**
 * @brief Task pool indexed by request ID.
 *
 */
using NetworkTaskIdMap = std::unordered_map<RequestId, NetworkTaskPtr>;

/**
 * @brief Map of task pools indexed by client connection handle.
 *
 */
using NetworkTaskConnectionMap = std::unordered_map<ConnectionHandle, NetworkTaskIdMap>;

/**
 * @brief Pool to store tasks.
 *
 * Allow to store and retreive tasks from the connection handle of the client
 * requesting it and the ID of the associated request.
 *
 */
class NetworkTaskMap
{
public:
    /**
     * @brief Add a task to the pool with the associated client and request ID.
     *
     * @param handle Connection handle of the client requesting the task.
     * @param id ID of the request starting the task.
     * @param task Abstract task (might be running or not).
     */
    void add(const ConnectionHandle &handle, const RequestId &id, NetworkTaskPtr task);

    /**
     * @brief Retreive a task using its client and request ID.
     *
     * @param handle Connection handle of the client requesting the task.
     * @param id ID of the request starting the task.
     * @return NetworkTask* Task or null if not found.
     */
    NetworkTask *find(const ConnectionHandle &handle, const RequestId &id) const;

    /**
     * @brief Call the given functor on each client, request ID, task.
     *
     * Functor: void(const ConnectionHandle&, const RequestId&, NetworkTask&).
     *
     * @tparam FunctorType Functor type.
     * @param functor Functor instance.
     */
    template<typename FunctorType>
    void forEach(FunctorType functor) const
    {
        for (const auto &pair : _tasks)
        {
            auto &handle = pair.first;
            auto &tasks = pair.second;
            for (const auto &child : tasks)
            {
                auto &id = child.first;
                auto &task = child.second;
                functor(handle, id, *task);
            }
        }
    }

    /**
     * @brief Call for each request ID, task required by the given client.
     *
     * Functor: void(const RequestId&, NetworkTask&).
     *
     * @tparam FunctorType Functor type.
     * @param handle Client connection handle.
     * @param functor Functor instance.
     */
    template<typename FunctorType>
    void forEach(const ConnectionHandle &handle, FunctorType functor) const
    {
        auto i = _tasks.find(handle);
        if (i == _tasks.end())
        {
            return;
        }
        auto &tasks = i->second;
        for (const auto &pair : tasks)
        {
            auto &id = pair.first;
            auto &task = pair.second;
            functor(id, *task);
        }
    }

    /**
     * @brief Remove tasks if the given functor returns true.
     *
     * Functor: bool(const ConnectionHandle&, const RequestId&, NetworkTask&).
     *
     * @tparam FunctorType Functor type.
     * @param functor Functor instance.
     */
    template<typename FunctorType>
    void removeIf(FunctorType functor)
    {
        for (auto i = _tasks.begin(); i != _tasks.end();)
        {
            auto &handle = i->first;
            auto &tasks = i->second;
            for (auto j = tasks.begin(); j != tasks.end();)
            {
                auto &id = j->first;
                auto &task = j->second;
                if (functor(handle, id, *task))
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

private:
    NetworkTaskConnectionMap _tasks;
};

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
    void addOrReplace(const ConnectionHandle &handle, const RequestId &id, NetworkTaskPtr task);

    /**
     * @brief Add a task if not already running.
     *
     * @param handle Client requesting the task.
     * @param id Task request ID from JSON-RPC.
     * @param task Task to register.
     * @return true No task with the same client / ID exists.
     * @return false Cannot register the task because already running.
     */
    bool addIfNotPresent(const ConnectionHandle &handle, const RequestId &id, NetworkTaskPtr task);

    /**
     * @brief Notify all task bound to a client when this one disconnects.
     *
     * @param handle Client handle.
     */
    void disconnect(const ConnectionHandle &handle) const;

    /**
     * @brief Cancel a task from its client and ID.
     *
     * @param handle Client connection handle.
     * @param id Task request ID.
     * @return true Task found and cancelled.
     * @return false Task not found.
     */
    bool cancel(const ConnectionHandle &handle, const RequestId &id) const;

    /**
     * @brief Poll all registered task.
     *
     * Polling a task checks if it is done, run cleanup if any and remove it
     * from the list.
     *
     */
    void poll();

private:
    NetworkTaskMap _tasks;
};
} // namespace brayns
