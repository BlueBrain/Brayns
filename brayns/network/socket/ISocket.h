/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/network/client/ClientRef.h>

namespace brayns
{
/**
 * @brief Interface to implement to be notified of network events.
 *
 * @note Can be called from a separated thread.
 *
 */
class ISocketListener
{
public:
    virtual ~ISocketListener() = default;

    /**
     * @brief Called when a request is received from a client.
     *
     * @param client Client sending request.
     * @param request Request received from client.
     */
    virtual void onRequest(const ClientRef &client, InputPacket request) = 0;

    /**
     * @brief Called when a new client connects.
     *
     * @param client Client ref.
     */
    virtual void onConnect(const ClientRef &client) = 0;

    /**
     * @brief Called when a client disconnects.
     *
     * @param client Client ref.
     */
    virtual void onDisconnect(const ClientRef &client) = 0;
};

/**
 * @brief Client or server socket.
 *
 */
class ISocket
{
public:
    virtual ~ISocket() = default;

    /**
     * @brief Used to start the network socket.
     *
     */
    virtual void start() = 0;
};
} // namespace brayns
