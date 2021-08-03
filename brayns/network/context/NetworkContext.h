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

#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Scene.h>

#include <brayns/network/entrypoint/EntrypointManager.h>
#include <brayns/network/socket/ConnectionManager.h>
#include <brayns/network/stream/StreamManager.h>
#include <brayns/network/tasks/NetworkTaskManager.h>

#include <brayns/parameters/ParametersManager.h>

#include <brayns/pluginapi/PluginAPI.h>

namespace brayns
{
class NetworkContext
{
public:
    NetworkContext(PluginAPI& api)
        : _api(&api)
        , _entrypoints(*this)
        , _stream(*this)
    {
    }

    PluginAPI& getApi() { return *_api; }

    EntrypointManager& getEntrypoints() { return _entrypoints; }

    ConnectionManager& getConnections() { return _connections; }

    StreamManager& getStream() { return _stream; }

    NetworkTaskManager& getTasks() { return _tasks; }

private:
    PluginAPI* _api;
    EntrypointManager _entrypoints;
    ConnectionManager _connections;
    StreamManager _stream;
    NetworkTaskManager _tasks;
};
} // namespace brayns