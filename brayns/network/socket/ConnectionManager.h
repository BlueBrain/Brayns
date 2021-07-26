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

#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include <brayns/common/log.h>

#include "NetworkSocket.h"

namespace brayns
{
using SocketPtr = std::shared_ptr<NetworkSocket>;

class ConnectionHandle
{
public:
    ConnectionHandle() = default;

    ConnectionHandle(SocketPtr socket)
        : _socket(std::move(socket))
    {
    }

    size_t getId() const { return size_t(_socket.get()); }

    bool operator==(const ConnectionHandle& other) const
    {
        return _socket == other._socket;
    }

    bool operator!=(const ConnectionHandle& other) const
    {
        return !(*this == other);
    }

private:
    SocketPtr _socket;
};
} // namespace brayns

namespace std
{
template <>
struct hash<brayns::ConnectionHandle>
{
    size_t operator()(const brayns::ConnectionHandle& handle) const
    {
        return handle.getId();
    }
};
} // namespace std

namespace brayns
{
using ConnectionBuffer = std::vector<InputPacket>;

struct Connection
{
    Connection() = default;

    Connection(SocketPtr socket)
        : socket(std::move(socket))
    {
    }

    SocketPtr socket;
    bool added = true;
    bool removed = false;
    ConnectionBuffer buffer;
};

using ConnectionMap = std::unordered_map<ConnectionHandle, Connection>;

using ConnectionCallback = std::function<void(const ConnectionHandle&)>;
using DisconnectionCallback = std::function<void(const ConnectionHandle&)>;
using RequestCallback =
    std::function<void(const ConnectionHandle&, const InputPacket&)>;

struct ConnectionListener
{
    ConnectionCallback onConnect;
    DisconnectionCallback onDisconnect;
    RequestCallback onRequest;
};

class ConnectionManager
{
public:
    bool isEmpty();
    size_t getConnectionCount();
    void connect(SocketPtr socket);
    void disconnect(const ConnectionHandle& handle);
    void receive(const ConnectionHandle& handle, InputPacket packet);
    void send(const ConnectionHandle& handle, const OutputPacket& packet);
    void broadcast(const OutputPacket& packet);
    void update();

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

class ConnectionRef
{
public:
    ConnectionRef() = default;

    ConnectionRef(ConnectionHandle handle, ConnectionManager& connections)
        : _handle(std::move(handle))
        , _connections(&connections)
    {
    }

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
            BRAYNS_ERROR << "Unexpected error during sending request.\n";
        }
    }

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
            BRAYNS_ERROR << "Unexpected error during broadcast.\n";
        }
    }

private:
    ConnectionHandle _handle;
    ConnectionManager* _connections = nullptr;
};
} // namespace brayns