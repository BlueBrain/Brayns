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
#include <unordered_map>
#include <vector>

#include "NetworkSocket.h"

namespace brayns
{
using ConnectionId = size_t;

struct RequestData
{
    ConnectionId id = 0;
    InputPacket packet;
};

using RequestBuffer = std::vector<RequestData>;

struct Connection
{
    Connection(const NetworkSocket& socket)
        : socket(socket)
    {
    }

    NetworkSocket socket;
    std::vector<InputPacket> buffer;
};

class ConnectionManager
{
public:
    bool isEmpty();
    size_t size();
    ConnectionId add(const NetworkSocket& socket);
    void remove(const NetworkSocket& socket);
    void remove(ConnectionId id);
    void bufferRequest(const NetworkSocket& socket, InputPacket packet);
    void bufferRequest(ConnectionId id, InputPacket packet);
    void send(ConnectionId id, const OutputPacket& packet);
    void broadcast(const OutputPacket& packet);
    RequestBuffer extractRequestBuffer();

private:
    std::mutex _mutex;
    std::unordered_map<ConnectionId, Connection> _connections;
};
} // namespace brayns