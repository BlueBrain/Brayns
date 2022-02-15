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

#include "WebSocket.h"

namespace
{
class WebSocketBuilder
{
public:
    static void build(Poco::Net::WebSocket &socket)
    {
        socket.setReceiveTimeout(Poco::Timespan());
        socket.setSendTimeout(Poco::Timespan());
    }
};

class WebSocketReceiver
{
public:
    static brayns::InputPacket receive(Poco::Net::WebSocket &socket)
    {
        Poco::Buffer<char> buffer(0);
        int flags = 0;
        try
        {
            socket.receiveFrame(buffer, flags);
        }
        catch (const Poco::Exception &e)
        {
            throw brayns::ConnectionClosedException("Error while receiving packet " + e.displayText());
        }
        brayns::InputPacket packet(std::move(buffer), flags);
        if (packet.isClose())
        {
            throw brayns::ConnectionClosedException("Close packet received");
        }
        if (packet.isEmpty())
        {
            throw brayns::ConnectionClosedException("Empty frame received");
        }
        return packet;
    }
};

class WebSocketSender
{
public:
    static void send(Poco::Net::WebSocket &socket, const brayns::OutputPacket &packet)
    {
        int size = 0;
        try
        {
            size = socket.sendFrame(packet.getData(), packet.getSize(), packet.getFlags());
        }
        catch (const Poco::Exception &e)
        {
            throw brayns::ConnectionClosedException("Error while sending packet " + e.displayText());
        }
        if (size < packet.getSize())
        {
            throw brayns::ConnectionClosedException("Cannot send frame entirely");
        }
    }
};
} // namespace

namespace brayns
{
InputPacket::InputPacket(Poco::Buffer<char> data, int flags)
    : _data(std::move(data))
    , _flags(flags)
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

OutputPacket::OutputPacket(const std::string &data)
    : _data(data.data())
    , _size(int(data.size()))
    , _flags(Poco::Net::WebSocket::FRAME_TEXT)
{
}

OutputPacket::OutputPacket(const void *data, int size)
    : _data(data)
    , _size(size)
    , _flags(Poco::Net::WebSocket::FRAME_BINARY)
{
}

bool OutputPacket::isEmpty() const
{
    return _size <= 0;
}

const void *OutputPacket::getData() const
{
    return _data;
}

int OutputPacket::getSize() const
{
    return _size;
}

int OutputPacket::getFlags() const
{
    return _flags;
}

ConnectionClosedException::ConnectionClosedException(const std::string &message)
    : std::runtime_error(message)
{
}

WebSocket::WebSocket(
    Poco::Net::HTTPClientSession &session,
    Poco::Net::HTTPRequest &request,
    Poco::Net::HTTPResponse &response)
    : _socket(session, request, response)
{
    WebSocketBuilder::build(_socket);
}

WebSocket::WebSocket(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response)
    : _socket(request, response)
{
    WebSocketBuilder::build(_socket);
}

void WebSocket::close()
{
    _socket.close();
}

InputPacket WebSocket::receive()
{
    return WebSocketReceiver::receive(_socket);
}

void WebSocket::send(const OutputPacket &packet)
{
    WebSocketSender::send(_socket, packet);
}
} // namespace brayns
