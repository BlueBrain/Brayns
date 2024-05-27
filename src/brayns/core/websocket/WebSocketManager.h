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

#include <functional>
#include <string>
#include <string_view>

#include "WebSocket.h"

#include <brayns/core/utils/IdGenerator.h>
#include <brayns/core/utils/Logger.h>

namespace brayns::experimental
{
struct WebSocketConnection
{
    std::function<WebSocketFrame()> receive;
    std::function<void(const WebSocketFrameView &)> send;
    std::function<void()> closeOk;
};

using ClientId = std::uint32_t;

struct RawResponse
{
    std::string_view data;
    bool binary;
};

struct RawRequest
{
    ClientId clientId;
    std::string data;
    bool binary;
    std::function<void(const RawResponse &)> respond;
};

struct WebSocketListener
{
    std::function<void(ClientId)> onConnect;
    std::function<void(ClientId)> onDisconnect;
    std::function<void(RawRequest)> onRequest;
};

class WebSocketManager
{
public:
    explicit WebSocketManager(WebSocketListener listener, Logger &logger);

    void handle(const WebSocketConnection &websocket);

private:
    WebSocketListener _listener;
    Logger *_logger;
    IdGenerator<ClientId> _clientIds;
};
}
