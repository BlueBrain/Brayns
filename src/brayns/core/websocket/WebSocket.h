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

#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

#include <Poco/Net/WebSocket.h>

namespace brayns::experimental
{
enum class WebSocketStatus
{
    NormalClose = Poco::Net::WebSocket::WS_NORMAL_CLOSE,
    PayloadNotAcceptable = Poco::Net::WebSocket::WS_PAYLOAD_NOT_ACCEPTABLE,
    PayloadTooBig = Poco::Net::WebSocket::WS_PAYLOAD_TOO_BIG,
    UnexpectedCondition = Poco::Net::WebSocket::WS_UNEXPECTED_CONDITION,
};

class WebSocketException : public std::runtime_error
{
public:
    explicit WebSocketException(WebSocketStatus status, const std::string &message);

    WebSocketStatus getStatus() const;

private:
    WebSocketStatus _status;
};

enum class WebSocketOpcode
{
    Continuation = Poco::Net::WebSocket::FRAME_OP_CONT,
    Text = Poco::Net::WebSocket::FRAME_OP_TEXT,
    Binary = Poco::Net::WebSocket::FRAME_OP_BINARY,
    Close = Poco::Net::WebSocket::FRAME_OP_CLOSE,
    Ping = Poco::Net::WebSocket::FRAME_OP_PING,
    Pong = Poco::Net::WebSocket::FRAME_OP_PONG,
};

struct WebSocketFrame
{
    WebSocketOpcode opcode;
    std::string data;
    bool finalFrame;
};

struct WebSocketFrameView
{
    WebSocketOpcode opcode;
    std::string_view data = {};
    bool finalFrame = true;
};

class WebSocket
{
public:
    explicit WebSocket(const Poco::Net::WebSocket &websocket);

    WebSocketFrame receive();
    void send(const WebSocketFrameView &frame);
    void close(WebSocketStatus status, std::string_view message = {});
    void close(const WebSocketException &e);

private:
    Poco::Net::WebSocket _websocket;
};
}
