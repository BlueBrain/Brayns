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

#include <mutex>

#include "ConnectionListener.h"
#include "ConnectionMap.h"
#include "NetworkSocket.h"

namespace brayns
{
/**
 * @brief Manage all active client connections.
 *
 * This object is thread safe (synchronized).
 *
 */
class ConnectionManager
{
public:
    /**
     * @brief Close all sockets.
     *
     */
    ~ConnectionManager();

    /**
     * @brief Retrieve the number of connected clients.
     *
     * @return size_t Connected client count.
     */
    size_t getConnectionCount();

    /**
     * @brief Add a client using its socket.
     *
     * @param socket Client socket.
     */
    void add(NetworkSocketPtr socket);

    /**
     * @brief Remove a client connection using its handle.
     *
     * @param handle Client handle.
     */
    void remove(const ConnectionHandle& handle);

    /**
     * @brief Store data packet received from a client.
     *
     * @param handle Emitter handle.
     * @param packet Data packet.
     */
    void receive(const ConnectionHandle& handle, InputPacket packet);

    /**
     * @brief Send a packet to a client.
     *
     * @param handle Receiver handle.
     * @param packet Data packet.
     */
    void send(const ConnectionHandle& handle, const OutputPacket& packet);

    /**
     * @brief Send a packet to all clients.
     *
     * @param packet Data packet.
     */
    void broadcast(const OutputPacket& packet);

    /**
     * @brief Send a packet to all clients except the source.
     *
     * @param source Packet emitter handle.
     * @param packet Data packet.
     */
    void broadcast(const ConnectionHandle& source, const OutputPacket& packet);

    /**
     * @brief Update all connections from the main loop.
     *
     * Process stored requests, incoming connections and closed sockets using
     * the provided callbacks.
     *
     */
    void update();

    /**
     * @brief Close all sockets.
     *
     */
    void closeAll();

    /**
     * @brief Shorcut to check if at least one client is connected.
     *
     * @return true Has clients.
     * @return false No clients.
     */
    bool isEmpty() { return getConnectionCount() == 0; }

    /**
     * @brief Set callback when a client connects.
     *
     * @param callback Connection callback.
     */
    void onConnect(ConnectionCallback callback)
    {
        _listener.onConnect = std::move(callback);
    }

    /**
     * @brief Set callback when a client disconnect.
     *
     * @param callback Disconnection callback.
     */
    void onDisconnect(DisconnectionCallback callback)
    {
        _listener.onDisconnect = std::move(callback);
    }

    /**
     * @brief Set callback when a request is received by
     *
     * @param callback
     */
    void onRequest(RequestCallback callback)
    {
        _listener.onRequest = std::move(callback);
    }

private:
    std::mutex _mutex;
    ConnectionMap _connections;
    ConnectionListener _listener;
};
} // namespace brayns