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

#include "NetworkSocket.h"

namespace brayns
{
InputPacket::InputPacket(const Poco::Buffer<char> &buffer, int flags)
    : _data(buffer.begin(), buffer.size())
    , _flags(flags)
{
}

bool InputPacket::isEmpty() const
{
    return _flags == 0 && _data.empty();
}

const std::string &InputPacket::getData() const
{
    return _data;
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

NetworkSocket::NetworkSocket(
    Poco::Net::HTTPClientSession &session,
    Poco::Net::HTTPRequest &request,
    Poco::Net::HTTPResponse &response)
    : _socket(session, request, response)
{
    _setupSocket();
}

NetworkSocket::NetworkSocket(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response)
    : _socket(request, response)
{
    _setupSocket();
}

void NetworkSocket::close()
{
    _socket.close();
}

InputPacket NetworkSocket::receive()
{
    Poco::Buffer<char> buffer(0);
    int flags = 0;
    try
    {
        _socket.receiveFrame(buffer, flags);
    }
    catch (Poco::Exception &e)
    {
        throw ConnectionClosedException(e.displayText());
    }
    InputPacket packet(buffer, flags);
    if (packet.isClose())
    {
        throw ConnectionClosedException("Close packet received");
    }
    if (packet.isEmpty())
    {
        throw ConnectionClosedException("Empty frame received");
    }
    return packet;
}

void NetworkSocket::send(const OutputPacket &packet)
{
    int size = 0;
    try
    {
        size = _socket.sendFrame(packet.getData(), packet.getSize(), packet.getFlags());
    }
    catch (Poco::Exception &e)
    {
        throw ConnectionClosedException(e.displayText());
    }
    if (size < packet.getSize())
    {
        throw ConnectionClosedException("Cannot send frame entirely");
    }
}

void NetworkSocket::_setupSocket()
{
    _socket.setReceiveTimeout(Poco::Timespan());
    _socket.setSendTimeout(Poco::Timespan());
}
} // namespace brayns
