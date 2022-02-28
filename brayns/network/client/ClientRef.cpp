/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "ClientRef.h"

#include <cassert>

namespace brayns
{
ClientRef::ClientRef(std::shared_ptr<WebSocket> socket)
    : _socket(std::move(socket))
{
    assert(_socket);
}

WebSocket &ClientRef::getSocket() const
{
    return *_socket;
}

size_t ClientRef::getId() const
{
    return size_t(_socket.get());
}

bool ClientRef::operator==(const ClientRef &other) const
{
    return _socket == other._socket;
}

bool ClientRef::operator!=(const ClientRef &other) const
{
    return _socket != other._socket;
}
} // namespace brayns

namespace std
{
std::ostream &operator<<(std::ostream &stream, const brayns::ClientRef &client)
{
    return stream << client.getId();
}

size_t hash<brayns::ClientRef>::operator()(const brayns::ClientRef &client) const
{
    return client.getId();
}
} // namespace std
