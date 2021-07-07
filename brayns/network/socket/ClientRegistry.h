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

#include <algorithm>
#include <vector>

#include "NetworkSocket.h"

namespace brayns
{
class ClientRegistry
{
public:
    bool isEmpty() const
    {
        return _sockets.empty();
    }

    void add(NetworkSocket& socket)
    {
        _sockets.push_back(&socket);
    }

    void remove(NetworkSocket& socket)
    {
        auto first = _sockets.begin();
        auto last = _sockets.end();
        auto i = std::find(first, last, &socket);
        if (i != last)
        {
            _sockets.erase(i);
        }
    }

    void broadcast(const OutputPacket& packet) const
    {
        for (auto socket : _sockets)
        {
            socket->send(packet);
        }
    }

private:
    std::vector<NetworkSocket*> _sockets;
};
} // namespace brayns