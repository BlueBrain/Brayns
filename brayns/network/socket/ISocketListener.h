/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/network/client/ClientRequest.h>

namespace brayns
{
/**
 * @brief Interface to implement to be notified of network events.
 *
 */
class ISocketListener
{
public:
    virtual ~ISocketListener() = default;

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

    /**
     * @brief Called when a request is received from a client.
     *
     * @param request Raw client request.
     */
    virtual void onRequest(ClientRequest request) = 0;
};
} // namespace brayns
