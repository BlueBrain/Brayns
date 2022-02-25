/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <stdexcept>

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/WebSocket.h>

#include "InputPacket.h"
#include "OutputPacket.h"

namespace brayns
{
/**
 * @brief Exception thrown by the socket when an operation cannot be achieved
 * because the connection is closed.
 *
 */
class ConnectionClosedException : public std::runtime_error
{
public:
    /**
     * @brief Construct an exception instance.
     *
     * @param message Description of the reason why the connection was closed.
     */
    ConnectionClosedException(const std::string &message);
};

/**
 * @brief WebSocket used to send an receive data.
 *
 */
class WebSocket
{
public:
    /**
     * @brief Construct a client side websocket from client session.
     *
     * @param session Client session.
     * @param request Client request.
     * @param response Server response.
     */
    WebSocket(
        Poco::Net::HTTPClientSession &session,
        Poco::Net::HTTPRequest &request,
        Poco::Net::HTTPResponse &response);

    /**
     * @brief Construct a server side websocket from client HTTP request.
     *
     * @param request Client request.
     * @param response Server response.
     */
    WebSocket(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);

    /**
     * @brief Close the socket.
     *
     */
    void close();

    /**
     * @brief Receive an input packet from the connected client.
     *
     * Block until data is received.
     *
     * @return InputPacket Data packet received from the client (always valid).
     * @throw ConnectionClosedException The client closed the connection.
     */
    InputPacket receive();

    /**
     * @brief Send an output packet to the connected client.
     *
     * @param packet Packet containing the data to send to the client.
     */
    void send(const OutputPacket &packet);

private:
    Poco::Net::WebSocket _socket;
};
} // namespace brayns
