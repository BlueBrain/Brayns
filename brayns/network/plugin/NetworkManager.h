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

#include <brayns/network/client/ClientManager.h>
#include <brayns/network/client/RequestBuffer.h>
#include <brayns/network/common/FrameExporter.h>
#include <brayns/network/entrypoint/EntrypointManager.h>
#include <brayns/network/socket/ISocket.h>
#include <brayns/network/stream/StreamManager.h>
#include <brayns/network/tasks/NetworkTaskManager.h>
#include <brayns/network/upload/ModelUploadManager.h>

#include <brayns/pluginapi/ExtensionPlugin.h>

namespace brayns
{
struct NetworkContext
{
    PluginAPI *api = nullptr;
    EntrypointManager entrypoints;
    ClientManager clients;
    RequestBuffer requests;
    StreamManager stream;
    NetworkTaskManager tasks;
    ModelUploadManager modelUploads;
    std::unique_ptr<ISocket> socket;
    FrameExporter frameExporter;
};

/**
 * @brief Network manager plugin.
 *
 * Provide the network action interface and core entrypoints.
 *
 */
class NetworkManager : public ExtensionPlugin
{
public:
    /**
     * @brief Construct the network manager.
     *
     */
    NetworkManager();

    /**
     * @brief Destroy the network manager.
     *
     */
    virtual ~NetworkManager();

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
     * @brief Process incoming network requests.
     *
     */
    void processRequests();

    /**
     * @brief Poll running tasks and update entrypoints.
     *
     */
    void update();

    /**
     * @brief Setup action interface and register core entrypoints.
     *
     */
    virtual void init() override;

    /**
     * @brief Notify entrypoints.
     *
     */
    virtual void preRender() override;

    /**
     * @brief Notify entrypoints and stream image if needed.
     *
     */
    virtual void postRender() override;

private:
    NetworkContext _context;
};
} // namespace brayns
