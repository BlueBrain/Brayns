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

void ConnectionManager::add(SocketPtr socket)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto& connection = _connections[socket];
    connection.socket = std::move(socket);
}

void ConnectionManager::remove(const ConnectionHandle& handle)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _connections.erase(handle);
}

void ConnectionManager::bufferRequest(const ConnectionHandle& handle,
                                      InputPacket packet)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto i = _connections.find(handle);
    if (i == _connections.end())
    {
        return;
    }
    auto& connection = i->second;
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

RequestBuffer ConnectionManager::extractRequestBuffer()
{
    std::lock_guard<std::mutex> lock(_mutex);
    RequestBuffer requests;
    for (auto& pair : _connections)
    {
        auto& handle = pair.first;
        auto& connection = pair.second;
        auto& buffer = connection.buffer;
        for (auto& packet : buffer)
        {
            requests.emplace_back();
            auto& request = requests.back();
            request.handle = handle;
            request.packet = std::move(packet);
        }
        buffer.clear();
    }
    return requests;
}
} // namespace brayns