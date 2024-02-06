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

#include <Poco/Net/WebSocket.h>

#include "IWebSocket.h"

namespace brayns
{
class WebSocket : public IWebSocket
{
public:
    /**
     * @brief Construct a websocket by injecting internal implementation.
     *
     * @param socket Connected websocket implementation.
     * @param id Unique ID for this socket to identify the client using it.
     */
    WebSocket(const Poco::Net::WebSocket &socket);

    /**
     * @brief Make sure each socket has a unique address.
     *
     */
    WebSocket(const WebSocket &) = delete;
    WebSocket(WebSocket &&) = delete;
    WebSocket &operator=(const WebSocket &) = delete;
    WebSocket &operator=(WebSocket &&) = delete;

    /**
     * @brief Get a unique ID for the socket.
     *
     * @return size_t Use the object address to identify the socket.
     */
    virtual size_t getId() const override;

    /**
     * @brief Close the socket.
     *
     */
    virtual void close() override;

    /**
     * @brief Receive an input packet from the connected client.
     *
     * Block until data is received.
     *
     * @return InputPacket Data packet received from the client (always valid).
     * @throw ConnectionClosedException The client closed the connection.
     */
    virtual InputPacket receive() override;

    /**
     * @brief Send an output packet to the connected client.
     *
     * @param packet Packet containing the data to send to the client.
     */
    virtual void send(const OutputPacket &packet) override;

private:
    Poco::Net::WebSocket _socket;
};
} // namespace brayns
