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

#include <vector>

#include "NetworkSocket.h"

namespace brayns
{
/**
 * @brief Buffer to store incoming requests and process them in the main loop.
 *
 */
using ConnectionBuffer = std::vector<InputPacket>;

/**
 * @brief Connection object to store the data of one client.
 *
 */
struct Connection
{
    /**
     * @brief Construct an invalid connection.
     *
     */
    Connection() = default;

    /**
     * @brief Construct a connection for the client using its socket.
     *
     * @param socket Socket opened by the client.
     */
    Connection(NetworkSocketPtr socket)
        : socket(std::move(socket))
    {
    }

    /**
     * @brief Client socket.
     *
     */
    NetworkSocketPtr socket;

    /**
     * @brief Check if the socket was added after the previous loop iteration.
     *
     * Used to trigger the connection callback.
     *
     */
    bool added = true;

    /**
     * @brief Check if the connection must be removed.
     *
     */
    bool removed = false;

    /**
     * @brief Request buffer associated with the client.
     *
     */
    ConnectionBuffer buffer;
};
} // namespace brayns