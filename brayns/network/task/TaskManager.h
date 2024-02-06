/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <deque>
#include <memory>

#include <brayns/network/client/ClientRef.h>
#include <brayns/network/jsonrpc/RequestId.h>

#include "ITask.h"

namespace brayns
{
/**
 * @brief Helper class to manage request processing.
 *
 */
class TaskManager
{
public:
    /**
     * @brief Register a new task.
     *
     * If the task has priority, it will be executed directly.
     *
     * Otherwise it will be queued and run on next call to run().
     *
     * @param task Task to run.
     */
    void add(std::unique_ptr<ITask> task);

    /**
     * @brief Run all registered tasks in the order they have been added.
     *
     */
    void run();

    /**
     * @brief Cancel all requests sent by client.
     *
     * @param client Client to cancel all tasks.
     */
    void disconnect(const ClientRef &client);

    /**
     * @brief Cancel request sent by client with given ID.
     *
     * @param client Client that requested the task.
     * @param id ID of the task to cancel.
     */
    void cancel(const ClientRef &client, const RequestId &id);

    /**
     * @brief Clear all tasks without running them.
     *
     */
    void clear();

private:
    std::deque<std::unique_ptr<ITask>> _tasks;
};
} // namespace brayns
