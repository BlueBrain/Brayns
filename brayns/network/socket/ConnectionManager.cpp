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
    return size() == 0;
}

size_t ConnectionManager::size()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _connections.size();
}

ConnectionId ConnectionManager::add(const NetworkSocket& socket)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto id = socket.getId();
    _connections.emplace(id, socket);
    return id;
}

void ConnectionManager::remove(const NetworkSocket& socket)
{
    remove(socket.getId());
}

void ConnectionManager::remove(ConnectionId id)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _connections.erase(id);
}

void ConnectionManager::bufferRequest(const NetworkSocket& socket,
                                      InputPacket packet)
{
    bufferRequest(socket.getId(), std::move(packet));
}

void ConnectionManager::bufferRequest(ConnectionId id, InputPacket packet)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto i = _connections.find(id);
    if (i == _connections.end())
    {
        return;
    }
    auto& connection = i->second;
    auto& buffer = connection.buffer;
    buffer.push_back(std::move(packet));
}

void ConnectionManager::send(ConnectionId id, const OutputPacket& packet)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto i = _connections.find(id);
    if (i == _connections.end())
    {
        return;
    }
    auto& connection = i->second;
    auto& socket = connection.socket;
    socket.send(packet);
}

void ConnectionManager::broadcast(const OutputPacket& packet)
{
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& pair : _connections)
    {
        auto& connection = pair.second;
        auto& socket = connection.socket;
        socket.send(packet);
    }
}

RequestBuffer ConnectionManager::extractRequestBuffer()
{
    std::lock_guard<std::mutex> lock(_mutex);
    RequestBuffer requests;
    for (auto& pair : _connections)
    {
        auto& connection = pair.second;
        auto& socket = connection.socket;
        auto id = socket.getId();
        auto& buffer = connection.buffer;
        for (auto& packet : buffer)
        {
            requests.emplace_back();
            auto& request = requests.back();
            request.id = id;
            request.packet = std::move(packet);
        }
        buffer.clear();
    }
    return requests;
}
} // namespace brayns