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

#pragma once

#include <brayns/network/interface/INetworkInterface.h>

namespace brayns
{
/**
 * @brief Wrapper around the network interface to start tasks.
 *
 */
class NetworkTaskLauncher
{
public:
    /**
     * @brief Construct a launcher using the network access.
     *
     * @param interface Network access.
     */
    NetworkTaskLauncher(INetworkInterface &interface)
        : _interface(interface)
    {
    }

    /**
     * @brief Start given task, guessing client and request ID from request.
     *
     * @tparam RequestType Request type with getClient() and getId() methods.
     * @tparam TaskType Task type subclass of NetworkTask.
     * @param request Request.
     * @param task Task.
     */
    template<typename RequestType, typename TaskType>
    void launch(const RequestType &request, std::unique_ptr<TaskType> task) const
    {
        auto &client = request.getClient();
        auto &id = request.getId();
        _interface.launchTask(client, id, std::move(task));
    }

    /**
     * @brief Start given task, guessing client and request ID from it.
     *
     * @tparam TaskType Task type subclass of NetworkTask with getClient() and getRequestId() methods.
     * @param task Task.
     */
    template<typename TaskType>
    void launch(std::unique_ptr<TaskType> task) const
    {
        auto &client = task->getClient();
        auto &id = task->getRequestId();
        _interface.launchTask(client, id, std::move(task));
    }

private:
    INetworkInterface &_interface;
};
} // namespace brayns
