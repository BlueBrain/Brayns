/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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
ClientRef::ClientRef(std::shared_ptr<IWebSocket> socket):
    _socket(std::move(socket))
{
    assert(_socket);
}

size_t ClientRef::getId() const
{
    return _socket->getId();
}

void ClientRef::disconnect() const
{
    _socket->close();
}

InputPacket ClientRef::receive() const
{
    return _socket->receive();
}

void ClientRef::send(const OutputPacket &packet) const
{
    _socket->send(packet);
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
