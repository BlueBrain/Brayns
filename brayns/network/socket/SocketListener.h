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

#include <brayns/network/binary/BinaryManager.h>
#include <brayns/network/client/ClientManager.h>
#include <brayns/network/entrypoint/EntrypointRegistry.h>
#include <brayns/network/task/TaskManager.h>

#include "ISocketListener.h"

namespace brayns
{
/**
 * @brief Socket listener implementation.
 *
 */
class SocketListener : public ISocketListener
{
public:
    /**
     * @brief Construct a listener with dependencies.
     *
     * @param binary Binary manager to buffer binary frames.
     * @param clients Client pool to update.
     * @param entrypoints Entrypoints to process requests.
     * @param tasks Tasks to schedule request processing.
     */
    SocketListener(
        BinaryManager &binary,
        ClientManager &clients,
        const EntrypointRegistry &entrypoints,
        TaskManager &tasks);

    /**
     * @brief Register new client and notify entrypoints.
     *
     * @param client Client ref.
     */
    virtual void onConnect(const ClientRef &client) override;

    /**
     * @brief Notify entrypoints and remove client.
     *
     * @param client Client ref.
     */
    virtual void onDisconnect(const ClientRef &client) override;

    /**
     * @brief Dispatch request to entrypoints.
     *
     * @param request Raw client request.
     */
    virtual void onRequest(ClientRequest request) override;

private:
    BinaryManager &_binary;
    ClientManager &_clients;
    const EntrypointRegistry &_entrypoints;
    TaskManager &_tasks;
};
} // namespace brayns
