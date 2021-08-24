/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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
using NetworkTaskIdMap = std::unordered_map<RequestId, NetworkTaskPtr>;

using NetworkTaskConnectionMap =
    std::unordered_map<ConnectionHandle, NetworkTaskIdMap>;

class NetworkTaskMap
{
public:
    void add(const ConnectionHandle& handle, const RequestId& id,
             NetworkTaskPtr task)
    {
        assert(task);
        _tasks[handle][id] = std::move(task);
    }

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