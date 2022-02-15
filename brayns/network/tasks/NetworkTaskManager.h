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

#include <memory>
#include <unordered_map>

#include <brayns/network/client/ClientRef.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>
#include <brayns/network/jsonrpc/RequestId.h>

#include "NetworkTask.h"

namespace brayns
{
/**
 * @brief Exception thrown when a task is already running.
 *
 */
class TaskAlreadyRunningException : public InvalidRequestException
{
public:
    /**
     * @brief Task already running for client and request ID.
     *
     * @param client Client starting the task.
     * @param id ID of the request starting the task.
     */
    TaskAlreadyRunningException(const ClientRef &client, const RequestId &id);
};

class TaskNotFoundException : public InvalidParamsException
{
public:
    /**
     * @brief No tasks running for given client.
     *
     * @param client Client starting the task.
     */
    TaskNotFoundException(const ClientRef &client);

    /**
     * @brief No tasks running for client and request ID.
     *
     * @param client Client starting the task.
     * @param id ID of the request starting the task.
     */
    TaskNotFoundException(const ClientRef &client, const RequestId &id);
};

class NetworkTaskManager
{
public:
    /**
     * @brief Register given task for given client and request ID.
     *
     * Call start() on given task.
     *
     * @param client Client starting the task.
     * @param id Task request ID.
     * @param task Task to run.
     * @throw TaskAlreadyRunningException Task with same client / ID running.
     */
    void add(const ClientRef &client, const RequestId &id, std::unique_ptr<NetworkTask> task);

    /**
     * @brief Cancel task using client and request ID.
     *
     * @param client Client that started the task.
     * @param id Request ID that started the task.
     * @throw TaskNotFoundException Task not found.
     */
    void cancel(const ClientRef &client, const RequestId &id);

    /**
     * @brief Notify tasks associated with given client of its deconnection.
     *
     * @param client Client that just disconnected.
     */
    void disconnect(const ClientRef &client);

    /**
     * @brief Poll all running tasks and remove the ones not running.
     *
     */
    void poll();

private:
    using MapFromRequestId = std::unordered_map<RequestId, std::unique_ptr<NetworkTask>>;
    using MapFromClient = std::unordered_map<ClientRef, MapFromRequestId>;

    MapFromClient _tasks;
};
} // namespace brayns
