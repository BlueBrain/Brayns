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

#include "ConnectionManager.h"

namespace
{
using namespace brayns;

class RequestBuffer
{
public:
    RequestBuffer() = default;

    RequestBuffer(size_t connectionCount) { _buffer.reserve(connectionCount); }

    void extract(const ConnectionHandle& handle, Connection& connection)
    {
        auto& buffer = connection.buffer;
        _buffer.emplace_back(handle, std::move(buffer));
        buffer.clear();
    }

    template <typename FunctorType>
    void forEach(FunctorType functor) const
    {
        for (const auto& pair : _buffer)
        {
            auto& handle = pair.first;
            auto& buffer = pair.second;
            for (const auto& packet : buffer)
            {
                functor(handle, packet);
            }
        }
    }

private:
    std::vector<std::pair<ConnectionHandle, ConnectionBuffer>> _buffer;
};

class ConnectionUpdater
{
public:
    static RequestBuffer update(ConnectionMap& connections,
                                const ConnectionListener& listener)
    {
        RequestBuffer buffer(connections.getConnectionCount());
        connections.removeIf([&](const auto& handle, auto& connection) {
            if (_tryDisconnect(handle, connection, listener))
            {
                return true;
            }
            if (_tryConnect(handle, connection, listener))
            {
                connection.added = false;
            }
            buffer.extract(handle, connection);
            return false;
        });
        return buffer;
    }

private:
    static bool _tryDisconnect(const ConnectionHandle& handle,
                               const Connection& connection,
                               const ConnectionListener& listener)
    {
        if (!connection.removed)
        {
            return false;
        }
        if (listener.onDisconnect)
        {
            listener.onDisconnect(handle);
        }
        return true;
    }

    static bool _tryConnect(const ConnectionHandle& handle,
                            const Connection& connection,
                            const ConnectionListener& listener)
    {
        if (!connection.added)
        {
            return false;
        }
        if (listener.onConnect)
        {
            listener.onConnect(handle);
        }
        return true;
    }
};
} // namespace

namespace brayns
{
ConnectionManager::~ConnectionManager()
{
    closeAll();
}

size_t ConnectionManager::getConnectionCount()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _connections.getConnectionCount();
}

void ConnectionManager::add(NetworkSocketPtr socket)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _connections.add(std::move(socket));
}

void ConnectionManager::remove(const ConnectionHandle& handle)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _connections.markAsRemoved(handle);
}

void ConnectionManager::receive(const ConnectionHandle& handle,
                                InputPacket packet)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto connection = _connections.find(handle);
    if (!connection)
    {
        return;
    }
    auto& buffer = connection->buffer;
    buffer.push_back(std::move(packet));
}

void ConnectionManager::send(const ConnectionHandle& handle,
                             const OutputPacket& packet)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto connection = _connections.find(handle);
    if (!connection)
    {
        return;
    }
    auto& socket = connection->socket;
    socket->send(packet);
}

void ConnectionManager::broadcast(const OutputPacket& packet)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _connections.forEach([&](const auto& handle, const auto& connection) {
        auto& socket = connection.socket;
        socket->send(packet);
    });
}

void ConnectionManager::broadcast(const ConnectionHandle& source,
                                  const OutputPacket& packet)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _connections.forEach([&](const auto& handle, const auto& connection) {
        if (handle == source)
        {
            return;
        }
        auto& socket = connection.socket;
        socket->send(packet);
    });
}

void ConnectionManager::update()
{
    RequestBuffer buffer;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        buffer = ConnectionUpdater::update(_connections, _listener);
    }
    if (!_listener.onRequest)
    {
        return;
    }
    buffer.forEach([this](const auto& handle, const auto& packet) {
        _listener.onRequest(handle, packet);
    });
}

void ConnectionManager::closeAll()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _connections.forEach([](auto& handle, auto& connection) {
        auto& socket = connection.socket;
        socket->close();
    });
}
} // namespace brayns