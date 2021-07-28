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

#include <stdexcept>

#include "NetworkTaskMap.h"

namespace brayns
{
class NetworkTaskManager
{
public:
    void addOrReplace(const ConnectionHandle& handle, const std::string& id,
                      NetworkTaskPtr task)
    {
        auto oldTask = _tasks.find(handle, id);
        if (oldTask)
        {
            oldTask->cancelAndWait();
        }
        _tasks.add(handle, id, std::move(task));
    }

    bool addIfNotPresent(const ConnectionHandle& handle, const std::string& id,
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

    void disconnect(const ConnectionHandle& handle) const
    {
        _tasks.forEach(handle, [&](auto&, auto& task) { task.onDisconnect(); });
    }

    bool cancel(const ConnectionHandle& handle, const std::string& id) const
    {
        auto task = _tasks.find(handle, id);
        if (!task)
        {
            return false;
        }
        task->cancel();
        return true;
    }

    void update()
    {
        _tasks.removeIf(
            [](auto&, auto&, auto& task)
            {
                bool complete = !task.isRunning();
                task.poll();
                return complete;
            });
    }

private:
    NetworkTaskMap _tasks;
};
} // namespace brayns