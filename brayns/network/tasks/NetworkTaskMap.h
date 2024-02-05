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

#include <cassert>
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
using NetworkTaskConnectionMap =
    std::unordered_map<ConnectionHandle, NetworkTaskIdMap>;

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
    void add(const ConnectionHandle& handle, const RequestId& id,
             NetworkTaskPtr task)
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
    NetworkTask* find(const ConnectionHandle& handle, const RequestId& id) const
    {
        auto i = _tasks.find(handle);
        if (i == _tasks.end())
        {
            return nullptr;
        }
        auto& tasks = i->second;
        auto j = tasks.find(id);
        if (j == tasks.end())
        {
            return nullptr;
        }
        auto& task = j->second;
        return task.get();
    }

    /**
     * @brief Call the given functor on each client, request ID, task.
     *
     * Functor: void(const ConnectionHandle&, const RequestId&, NetworkTask&).
     *
     * @tparam FunctorType Functor type.
     * @param functor Functor instance.
     */
    template <typename FunctorType>
    void forEach(FunctorType functor) const
    {
        for (const auto& pair : _tasks)
        {
            auto& handle = pair.first;
            auto& tasks = pair.second;
            for (const auto& child : tasks)
            {
                auto& id = child.first;
                auto& task = child.second;
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
    template <typename FunctorType>
    void forEach(const ConnectionHandle& handle, FunctorType functor) const
    {
        auto i = _tasks.find(handle);
        if (i == _tasks.end())
        {
            return;
        }
        auto& tasks = i->second;
        for (const auto& pair : tasks)
        {
            auto& id = pair.first;
            auto& task = pair.second;
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
    template <typename FunctorType>
    void removeIf(FunctorType functor)
    {
        for (auto i = _tasks.begin(); i != _tasks.end();)
        {
            auto& handle = i->first;
            auto& tasks = i->second;
            for (auto j = tasks.begin(); j != tasks.end();)
            {
                auto& id = j->first;
                auto& task = j->second;
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
} // namespace brayns