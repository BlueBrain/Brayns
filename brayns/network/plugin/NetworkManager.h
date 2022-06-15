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
#include <brayns/network/client/RequestBuffer.h>
#include <brayns/network/entrypoint/EntrypointRegistry.h>
#include <brayns/network/socket/ISocket.h>
#include <brayns/network/stream/StreamManager.h>
#include <brayns/network/task/TaskManager.h>

#include <brayns/pluginapi/IPlugin.h>
#include <brayns/pluginapi/PluginAPI.h>

namespace brayns
{
struct NetworkContext
{
    PluginAPI &api;
    std::unique_ptr<INetworkInterface> interface;
    std::unique_ptr<ISocket> socket;
    BinaryManager binary;
    ClientManager clients;
    EntrypointRegistry entrypoints;
    StreamManager stream;
    TaskManager tasks;

    NetworkContext(PluginAPI &pluginAPI);
};

/**
 * @brief Network manager plugin.
 *
 * Provide the network action interface and core entrypoints.
 *
 */
class NetworkManager : public IPlugin
{
public:
    /**
     * @brief Construct the network manager.
     *
     */
    NetworkManager(PluginAPI &api);

    /**
     * @brief Get the interface to access network from plugins.
     *
     * @return INetworkInterface& Network interface.
     */
    INetworkInterface &getInterface();

    /**
     * @brief Load schemas and call onCreate() of all entrypoints.
     *
     * This method must be separated from the construction because it can only
     * be called once all plugins and entrypoints are registered to have correct
     * schemas.
     *
     */
    void start();

    /**
     * @brief Initialize network components.
     *
     */
    virtual void onCreate() override;

    /**
     * @brief Register core entrypoints.
     *
     * @param interface Network access.
     */
    virtual void registerEntrypoints(INetworkInterface &interface) override;

    /**
     * @brief Notify entrypoints.
     *
     */
    virtual void onPreRender() override;

    /**
     * @brief Notify entrypoints and stream image if needed.
     *
     */
    virtual void onPostRender() override;

private:
    NetworkContext _context;
};
} // namespace brayns
