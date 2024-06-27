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

#include <limits>
#include <memory>
#include <optional>

#include <Poco/Net/HTTPServer.h>

#include <brayns/core/utils/Logger.h>

#include "RequestQueue.h"
#include "WebSocket.h"

namespace brayns::experimental
{
struct SslSettings
{
    std::string privateKeyFile;
    std::string certificateFile;
    std::string caLocation = {};
    std::string privateKeyPassphrase = {};
};

struct WebSocketServerSettings
{
    std::string host = "localhost";
    std::uint16_t port = 5000;
    std::size_t maxThreadCount = 1;
    std::size_t maxQueueSize = 64;
    std::size_t maxFrameSize = std::numeric_limits<int>::max();
    std::optional<SslSettings> ssl = std::nullopt;
};

class WebSocketServer
{
public:
    explicit WebSocketServer(std::unique_ptr<Poco::Net::HTTPServer> server, std::unique_ptr<RequestQueue> requests);
    ~WebSocketServer();

    WebSocketServer(const WebSocketServer &) = delete;
    WebSocketServer(WebSocketServer &&) = default;
    WebSocketServer &operator=(const WebSocketServer &) = delete;
    WebSocketServer &operator=(WebSocketServer &&) = default;

    std::vector<RawRequest> waitForRequests();

private:
    std::unique_ptr<Poco::Net::HTTPServer> _server;
    std::unique_ptr<RequestQueue> _requests;
};

WebSocketServer startServer(const WebSocketServerSettings &settings, Logger &logger);
}
