/* Copyright (c) 2021 EPFL/Blue Brain Project
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
class ConnectionManager
{
public:
    ~ConnectionManager();

    size_t getConnectionCount();
    void add(NetworkSocketPtr socket);
    void remove(const ConnectionHandle& handle);
    void receive(const ConnectionHandle& handle, InputPacket packet);
    void send(const ConnectionHandle& handle, const OutputPacket& packet);
    void broadcast(const OutputPacket& packet);
    void broadcast(const ConnectionHandle& source, const OutputPacket& packet);
    void processRequests();
    void closeAll();

    bool isEmpty() { return getConnectionCount() == 0; }

    void onConnect(ConnectionCallback callback)
    {
        _listener.onConnect = std::move(callback);
    }

    void onDisconnect(DisconnectionCallback callback)
    {
        _listener.onDisconnect = std::move(callback);
    }

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