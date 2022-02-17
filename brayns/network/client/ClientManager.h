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

#include <mutex>
#include <unordered_set>

#include "ClientRef.h"

namespace brayns
{
/**
 * @brief Manage all connected clients.
 *
 * This object is thread safe (synchronized).
 *
 */
class ClientManager
{
public:
    /**
     * @brief Close all sockets.
     *
     */
    ~ClientManager();

    /**
     * @brief Shorcut to check if at least one client is connected.
     *
     * @return true Has clients.
     * @return false No clients.
     */
    bool isEmpty();

    /**
     * @brief Add a client.
     *
     * @param client Client ref.
     */
    void add(ClientRef client);

    /**
     * @brief Remove a client connection.
     *
     * @param client Client ref.
     */
    void remove(const ClientRef &client);

    /**
     * @brief Send a packet to all clients except the source if specified.
     *
     * @param packet Data packet.
     */
    void broadcast(const OutputPacket &packet);

    /**
     * @brief Close all sockets.
     *
     */
    void closeAll();

private:
    std::mutex _mutex;
    std::unordered_set<ClientRef> _clients;
};
} // namespace brayns
