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

#include <limits>
#include <optional>

#include <fmt/format.h>

#include <Poco/Buffer.h>
#include <Poco/Net/NetException.h>

namespace
{
using namespace brayns::experimental;

WebSocketFrame receiveFrame(Poco::Net::WebSocket &websocket)
{
    auto flags = 0;
    auto buffer = Poco::Buffer<char>(0);

    websocket.receiveFrame(buffer, flags);

    auto finalFrame = flags & Poco::Net::WebSocket::FRAME_FLAG_FIN;

    auto opcode = flags & Poco::Net::WebSocket::FRAME_OP_BITMASK;

    return {
        .opcode = static_cast<WebSocketOpcode>(opcode),
        .data = {buffer.begin(), buffer.size()},
        .finalFrame = static_cast<bool>(finalFrame),
    };
}

int getFlags(WebSocketOpcode opcode, bool finalFrame)
{
    auto flags = 0;

    if (finalFrame)
    {
        flags |= Poco::Net::WebSocket::FRAME_FLAG_FIN;
    }

    return flags | static_cast<int>(opcode);
}

WebSocketStatus getStatus(int errorCode)
{
    switch (errorCode)
    {
    case Poco::Net::WebSocket::WS_ERR_PAYLOAD_TOO_BIG:
        return WebSocketStatus::PayloadTooBig;
    case Poco::Net::WebSocket::WS_ERR_INCOMPLETE_FRAME:
        return WebSocketStatus::PayloadNotAcceptable;
    default:
        return WebSocketStatus::UnexpectedCondition;
    }
}

WebSocketException websocketException(const Poco::Exception &e)
{
    auto status = getStatus(e.code());
    auto message = e.displayText();
    return WebSocketException(status, message);
}
}

namespace brayns::experimental
{
WebSocketException::WebSocketException(WebSocketStatus status, const std::string &message):
    runtime_error(message),
    _status(status)
{
}

WebSocketStatus WebSocketException::getStatus() const
{
    return _status;
}

WebSocket::WebSocket(const Poco::Net::WebSocket &websocket):
    _websocket(websocket)
{
}

WebSocketFrame WebSocket::receive()
{
    try
    {
        return receiveFrame(_websocket);
    }
    catch (const Poco::Exception &e)
    {
        throw websocketException(e);
    }
}

void WebSocket::send(const WebSocketFrameView &frame)
{
    const auto &[opcode, data, finalFrame] = frame;

    auto flags = getFlags(opcode, finalFrame);

    if (data.size() > std::numeric_limits<int>::max())
    {
        throw std::invalid_argument("Payload too big");
    }

    auto size = static_cast<int>(data.size());

    try
    {
        _websocket.sendFrame(data.data(), size, flags);
    }
    catch (const Poco::Exception &e)
    {
        throw websocketException(e);
    }
}

void WebSocket::close(WebSocketStatus status, std::string_view message)
{
    try
    {
        _websocket.shutdown(static_cast<Poco::UInt16>(status), std::string(message));
    }
    catch (...)
    {
    }
}

void WebSocket::close(const WebSocketException &e)
{
    close(e.getStatus(), e.what());
}
}
