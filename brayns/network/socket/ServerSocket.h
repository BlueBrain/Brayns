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

#include <memory>

#include <Poco/Net/HTTPServer.h>

#include <brayns/parameters/NetworkParameters.h>

#include "ISocket.h"
#include "SocketManager.h"

namespace brayns
{
/**
 * @brief Server side implementation of the Network interface.
 *
 */
class ServerSocket : public ISocket
{
public:
    /**
     * @brief Construct a server socket.
     *
     * Start an HTTP server in a separated thread that listen on Brayns HTTP
     * server port. On each client request, a WebSocket communication is opened
     * using the HTTP request and response.
     *
     * @param parameters SSL and URI parameters.
     * @param listener Listener to receive network events.
     */
    ServerSocket(const NetworkParameters &parameters, std::unique_ptr<ISocketListener> listener);

    /**
     * @brief Start the server to accept incoming request.
     *
     */
    virtual void start() override;

    /**
     * @brief Close all connections.
     *
     */
    virtual void stop() override;

private:
    std::unique_ptr<Poco::Net::HTTPServer> _server;
    SocketManager _manager;
};
} // namespace brayns
