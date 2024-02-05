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

#include <memory>

#include <Poco/Net/HTTPServer.h>

#include "NetworkInterface.h"

namespace brayns
{
/**
 * @brief Server side implementation of the Network interface.
 *
 */
class ServerInterface : public NetworkInterface
{
public:
    /**
     * @brief Construct the server interface.
     *
     * Start an HTTP server in a separated thread that listen on Brayns HTTP
     * server port. On each client request, a WebSocket communication is opened
     * using the HTTP request and response and forwarded to the base class run
     * method.
     *
     * @param context Network context reference.
     */
    ServerInterface(NetworkContext& context);

    /**
     * @brief Start the server to accept incoming request.
     *
     */
    virtual void start() override;

private:
    std::unique_ptr<Poco::Net::HTTPServer> _server;
};
} // namespace brayns