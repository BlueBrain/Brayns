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

#include "ConnectionManager.h"

namespace
{
using namespace brayns;

class RequestBuffer
{
public:
    RequestBuffer() = default;

    RequestBuffer(size_t connectionCount) { _buffer.reserve(connectionCount); }

    void add(const ConnectionHandle& handle, ConnectionBuffer buffer)
    {
        _buffer.emplace_back(handle, std::move(buffer));
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
        RequestBuffer requests(connections.size());
        for (auto i = connections.begin(); i != connections.end();)
        {
            auto& handle = i->first;
            auto& connection = i->second;
            if (_tryDisconnect(handle, connection, listener))
            {
                i = connections.erase(i);
                continue;
            }
            if (_tryConnect(handle, connection, listener))
            {
                connection.added = false;
            }
            auto& buffer = connection.buffer;
            requests.add(handle, std::move(buffer));
            buffer.clear();
            ++i;
        }
        return requests;
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
bool ConnectionManager::isEmpty()
{
    return getConnectionCount() == 0;
}

size_t ConnectionManager::getConnectionCount()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _connections.size();
}

void ConnectionManager::connect(SocketPtr socket)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto& connection = _connections[socket];
    connection.socket = std::move(socket);
}

void ConnectionManager::disconnect(const ConnectionHandle& handle)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto i = _connections.find(handle);
    if (i == _connections.end())
    {
        return;
    }
    auto& connection = i->second;
    connection.removed = true;
}

void ConnectionManager::receive(const ConnectionHandle& handle,
                                InputPacket packet)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto i = _connections.find(handle);
    if (i == _connections.end())
    {
        return;
    }
    auto& connection = i->second;
    if (connection.removed)
    {
        return;
    }
    auto& buffer = connection.buffer;
    buffer.push_back(std::move(packet));
}

void ConnectionManager::send(const ConnectionHandle& handle,
                             const OutputPacket& packet)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto i = _connections.find(handle);
    if (i == _connections.end())
    {
        return;
    }
    auto& connection = i->second;
    if (connection.removed)
    {
        return;
    }
    auto& socket = connection.socket;
    socket->send(packet);
}

void ConnectionManager::broadcast(const OutputPacket& packet)
{
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& pair : _connections)
    {
        auto& connection = pair.second;
        auto& socket = connection.socket;
        socket->send(packet);
    }
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
    buffer.forEach([this](const auto& handle, const auto& packet)
                   { _listener.onRequest(handle, packet); });
}
} // namespace brayns