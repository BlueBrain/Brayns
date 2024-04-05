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

#include <stdexcept>

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
    explicit ConnectionClosedException(const std::string &message):
        std::runtime_error(message)
    {
    }
};

/**
 * @brief WebSocket interface used to send an receive data with a client.
 *
 */
class IWebSocket
{
public:
    virtual ~IWebSocket() = default;

    /**
     * @brief Get a unique ID for the socket.
     *
     * @return size_t An ID that is unique among all connected sockets.
     */
    virtual size_t getId() const = 0;

    /**
     * @brief Close the socket.
     *
     */
    virtual void close() = 0;

    /**
     * @brief Receive an input packet from the connected client.
     *
     * Block until data is received.
     *
     * @return InputPacket Data packet received from the client.
     * @throw ConnectionClosedException The client closed the connection.
     */
    virtual InputPacket receive() = 0;

    /**
     * @brief Send an output packet to the connected client.
     *
     * @param packet Packet containing the data to send to the client.
     */
    virtual void send(const OutputPacket &packet) = 0;
};
} // namespace brayns
