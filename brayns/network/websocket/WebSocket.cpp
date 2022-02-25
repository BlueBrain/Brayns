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
            throw brayns::ConnectionClosedException("Error while receiving packet: '" + e.displayText() + "'");
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
        auto data = packet.getData();
        auto flags = packet.getFlags();
        try
        {
            size = socket.sendFrame(data.data(), data.size(), flags);
        }
        catch (const Poco::Exception &e)
        {
            throw brayns::ConnectionClosedException("Error while sending packet: '" + e.displayText() + "'");
        }
        if (size != data.size())
        {
            throw brayns::ConnectionClosedException("Cannot send frame entirely");
        }
    }
};
} // namespace

namespace brayns
{
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
