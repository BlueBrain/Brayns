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

#include <brayns/common/Log.h>

#include "ConnectionHandle.h"
#include "ConnectionManager.h"

namespace brayns
{
/**
 * @brief Wrapper around the connection manager and handle.
 *
 * Use a pointer to the manager and a handle to synchronize operations.
 *
 */
class ConnectionRef
{
public:
    /**
     * @brief Construct an invalid connection.
     *
     */
    ConnectionRef() = default;

    /**
     * @brief Construct a connection reference from the client and manager.
     *
     * @param handle Client handle.
     * @param connections Connection manager.
     */
    ConnectionRef(ConnectionHandle handle, ConnectionManager& connections)
        : _handle(std::move(handle))
        , _connections(&connections)
    {
    }

    /**
     * @brief Get the client handle.
     *
     * @return const ConnectionHandle& Client connection handle.
     */
    const ConnectionHandle& getHandle() const { return _handle; }

    /**
     * @brief Send a packet to the client.
     *
     * @param packet Data packet.
     */
    void send(const OutputPacket& packet) const
    {
        if (!_connections)
        {
            return;
        }
        try
        {
            _connections->send(_handle, packet);
        }
        catch (...)
        {
            Log::error("Unexpected error during sending request.");
        }
    }

    /**
     * @brief Send a packet to all clients.
     *
     * @param packet Data packet.
     */
    void broadcast(const OutputPacket& packet) const
    {
        if (!_connections)
        {
            return;
        }
        try
        {
            _connections->broadcast(packet);
        }
        catch (...)
        {
            Log::error("Unexpected error during broadcast.");
        }
    }

    /**
     * @brief Send a packet to all client except this one.
     *
     * @param packet Data packet.
     */
    void broadcastToOtherClients(const OutputPacket& packet) const
    {
        if (!_connections)
        {
            return;
        }
        try
        {
            _connections->broadcast(_handle, packet);
        }
        catch (...)
        {
            Log::error("Unexpected error during broadcast to others.");
        }
    }

private:
    ConnectionHandle _handle;
    ConnectionManager* _connections = nullptr;
};
} // namespace brayns