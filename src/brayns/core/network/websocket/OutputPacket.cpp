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

#include "WebSocket.h"

#include <Poco/Net/WebSocket.h>

namespace brayns
{
OutputPacket OutputPacket::fromBinary(std::string_view data)
{
    return {data, Poco::Net::WebSocket::FRAME_BINARY};
}

OutputPacket OutputPacket::fromText(std::string_view data)
{
    return {data, Poco::Net::WebSocket::FRAME_TEXT};
}

OutputPacket::OutputPacket(std::string_view data, int flags):
    _data(data),
    _flags(flags)
{
}

std::string_view OutputPacket::getData() const
{
    return _data;
}

int OutputPacket::getFlags() const
{
    return _flags;
}

bool OutputPacket::isBinary() const
{
    return _flags == Poco::Net::WebSocket::FRAME_BINARY;
}

bool OutputPacket::isText() const
{
    return _flags == Poco::Net::WebSocket::FRAME_TEXT;
}
} // namespace brayns
