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

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

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
struct RequestData
{
    ConnectionHandle handle;
    InputPacket packet;
};

using RequestBuffer = std::vector<RequestData>;

struct Connection
{
    Connection() = default;

    Connection(SocketPtr socket)
        : socket(std::move(socket))
    {
    }

    SocketPtr socket;
    std::vector<InputPacket> buffer;
};

class ConnectionManager
{
public:
    bool isEmpty();
    size_t getConnectionCount();
    void add(SocketPtr socket);
    void remove(const ConnectionHandle& handle);
    void bufferRequest(const ConnectionHandle& handle, InputPacket packet);
    void send(const ConnectionHandle& handle, const OutputPacket& packet);
    void broadcast(const OutputPacket& packet);
    RequestBuffer extractRequestBuffer();

private:
    std::mutex _mutex;
    std::unordered_map<ConnectionHandle, Connection> _connections;
};
} // namespace brayns