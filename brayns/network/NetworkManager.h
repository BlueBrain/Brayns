/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/network/client/ClientManager.h>
#include <brayns/network/entrypoint/EntrypointRegistry.h>
#include <brayns/network/socket/ISocket.h>
#include <brayns/network/task/TaskManager.h>

#include <brayns/pluginapi/PluginAPI.h>

#include "INetworkInterface.h"
#include "NetworkMonitor.h"

namespace brayns
{
/**
 * @brief Network manager.
 *
 * Provide a network interface and register core entrypoints.
 *
 */
class NetworkManager : public INetworkInterface
{
public:
    /**
     * @brief Construct the network manager.
     *
     */
    explicit NetworkManager(PluginAPI &api);

    /**
     * @brief Start server / client until stop request is received.
     *
     */
    void run();

    /**
     * @brief Register an entrypoint.
     *
     * @param entrypoint Entrypoint to register.
     */
    virtual void registerEntrypoint(EntrypointRef entrypoint) override;

    /**
     * @brief Poll socket to receive incoming messages.
     *
     */
    virtual void poll() override;

    /**
     * @brief Stop the network loop.
     *
     */
    virtual void stop() override;

private:
    std::unique_ptr<ISocket> _socket;
    ClientManager _clients;
    EntrypointRegistry _entrypoints;
    TaskManager _tasks;
    bool _running = false;
    NetworkMonitor _monitor;
};
} // namespace brayns
