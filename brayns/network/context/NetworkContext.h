/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Scene.h>

#include <brayns/network/binary/BinaryRequestManager.h>
#include <brayns/network/entrypoint/EntrypointManager.h>
#include <brayns/network/socket/ConnectionManager.h>
#include <brayns/network/stream/StreamManager.h>
#include <brayns/network/tasks/NetworkTaskManager.h>

#include <brayns/parameters/ParametersManager.h>

#include <brayns/pluginapi/PluginAPI.h>

namespace brayns
{
/**
 * @brief Holds the network data shared between all entrypoints.
 *
 */
class NetworkContext
{
public:
    /**
     * @brief Construct a new context used by the network with API access.
     *
     * @param api Brayns API.
     */
    NetworkContext(PluginAPI& api)
        : _api(&api)
        , _entrypoints(*this)
        , _stream(*this)
    {
    }

    /**
     * @brief Get Brayns API to access engine, parameters, etc.
     *
     * @return PluginAPI& Brayns API.
     */
    PluginAPI& getApi() { return *_api; }

    /**
     * @brief Get registered entrypoints.
     *
     * @return EntrypointManager& Entrypoint manager with all entrypoints.
     */
    EntrypointManager& getEntrypoints() { return _entrypoints; }

    /**
     * @brief Get all connected clients with the buffered requests.
     *
     * @return ConnectionManager& Connection manager with all clients.
     */
    ConnectionManager& getConnections() { return _connections; }

    /**
     * @brief Get the image stream monitor.
     *
     * @return StreamManager& Stream manager to monitor image stream.
     */
    StreamManager& getStream() { return _stream; }

    /**
     * @brief Get the tasks running in parallel of the main loop.
     *
     * @return NetworkTaskManager& Task manager with all running tasks.
     */
    NetworkTaskManager& getTasks() { return _tasks; }

    /**
     * @brief Get the binary model upload manager.
     *
     * @return BinaryRequestManager& Binary manager with tasks and chunks.
     */
    BinaryRequestManager& getBinary() { return _binary; }

private:
    PluginAPI* _api;
    EntrypointManager _entrypoints;
    ConnectionManager _connections;
    StreamManager _stream;
    NetworkTaskManager _tasks;
    BinaryRequestManager _binary;
};
} // namespace brayns