/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 *
 * Responsible Authors: Daniel.Nachbaur@epfl.ch
 *                      Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/network/client/ClientRef.h>
#include <brayns/network/entrypoint/EntrypointRef.h>
#include <brayns/network/jsonrpc/JsonRpcMessages.h>
#include <brayns/network/jsonrpc/JsonRpcRequest.h>
#include <brayns/network/tasks/NetworkTask.h>

namespace brayns
{
/**
 * Access to network API.
 *
 */
class INetworkInterface
{
public:
    virtual ~INetworkInterface() = default;

    /**
     * @brief Register an entrypoint.
     *
     * @param entrypoint Entrypoint to register.
     */
    virtual void addEntrypoint(EntrypointRef entrypoint) = 0;

    /**
     * @brief Register and start a task.
     *
     * @param client Client requesting the task.
     * @param id Task request ID.
     * @param task Task to execute.
     */
    virtual void launchTask(const ClientRef &client, const RequestId &id, std::unique_ptr<NetworkTask> task) = 0;

    /**
     * @brief Send a notification to all clients except source if set.
     *
     * @param message Notification message.
     * @param source Optional source of the notification.
     */
    virtual void notify(const NotificationMessage &message, const ClientRef &source = {}) = 0;
};
} // namespace brayns
