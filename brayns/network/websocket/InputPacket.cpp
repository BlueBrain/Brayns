/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "InputPacket.h"

#include <Poco/Net/WebSocket.h>

namespace brayns
{
InputPacket InputPacket::fromBinary(std::string_view data)
{
    return {{data.data(), data.size()}, Poco::Net::WebSocket::FRAME_OP_BINARY};
}

InputPacket InputPacket::fromText(std::string_view data)
{
    return {{data.data(), data.size()}, Poco::Net::WebSocket::FRAME_OP_TEXT};
}

InputPacket::InputPacket(Poco::Buffer<char> data, int flags):
    _data(std::move(data)),
    _flags(flags)
{
}

bool InputPacket::isEmpty() const
{
    return _flags == 0 && _data.empty();
}

std::string_view InputPacket::getData() const
{
    return {_data.begin(), _data.size()};
}

bool InputPacket::isBinary() const
{
    return _flags & Poco::Net::WebSocket::FRAME_OP_BINARY;
}

bool InputPacket::isText() const
{
    return _flags & Poco::Net::WebSocket::FRAME_OP_TEXT;
}

bool InputPacket::isClose() const
{
    return _flags & Poco::Net::WebSocket::FRAME_OP_CLOSE;
}
} // namespace brayns
