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
#include <stdexcept>

#include <brayns/core/utils/Log.h>

namespace
{
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

class ReceiveLogger
{
public:
    static void log(const brayns::InputPacket &packet, size_t id)
    {
        if (packet.isBinary())
        {
            _logBinary(packet, id);
            return;
        }
        if (packet.isText())
        {
            _logText(packet, id);
            return;
        }
        brayns::Log::error("Received invalid packet (neither text nor binary).");
    }

private:
    static void _logBinary(const brayns::InputPacket &packet, size_t id)
    {
        auto data = packet.getData();
        auto size = data.size();
        brayns::Log::debug("Received binary frame of {} bytes from client {}.", size, id);
    }

    static void _logText(const brayns::InputPacket &packet, size_t id)
    {
        auto data = packet.getData();
        brayns::Log::debug("Received text frame from client {}: '{}'.", id, data);
    }
};

class WebSocketSender
{
public:
    static void send(Poco::Net::WebSocket &socket, const brayns::OutputPacket &packet)
    {
        auto data = packet.getData();
        auto size = data.size();
        constexpr auto limit = static_cast<size_t>(std::numeric_limits<int>::max());
        if (size > limit)
        {
            throw std::invalid_argument("Output packet size too big: " + std::to_string(size));
        }
        auto buffer = data.data();
        auto flags = packet.getFlags();
        try
        {
            socket.sendFrame(buffer, static_cast<int>(size), flags);
        }
        catch (const Poco::Exception &e)
        {
            throw brayns::ConnectionClosedException("Error while sending packet: '" + e.displayText() + "'");
        }
    }
};

class SendLogger
{
public:
    static void log(const brayns::OutputPacket &packet, size_t id)
    {
        if (packet.isBinary())
        {
            _logBinary(packet, id);
            return;
        }
        if (packet.isText())
        {
            _logText(packet, id);
            return;
        }
        brayns::Log::error("Trying to send invalid packet (neither text nor binary).");
    }

private:
    static void _logBinary(const brayns::OutputPacket &packet, size_t id)
    {
        auto data = packet.getData();
        auto size = data.size();
        brayns::Log::debug("Sending binary frame of {} bytes to client {}.", size, id);
    }

    static void _logText(const brayns::OutputPacket &packet, size_t id)
    {
        auto data = packet.getData();
        brayns::Log::debug("Sending text frame to client {}: '{}'.", id, data);
    }
};
} // namespace

namespace brayns
{
WebSocket::WebSocket(const Poco::Net::WebSocket &socket):
    _socket(socket)
{
    _socket.setReceiveTimeout(Poco::Timespan());
    _socket.setSendTimeout(Poco::Timespan());
}

size_t WebSocket::getId() const
{
    return reinterpret_cast<size_t>(this);
}

void WebSocket::close()
{
    _socket.close();
}

InputPacket WebSocket::receive()
{
    auto id = getId();
    try
    {
        auto packet = WebSocketReceiver::receive(_socket);
        ReceiveLogger::log(packet, id);
        return packet;
    }
    catch (const brayns::ConnectionClosedException &e)
    {
        brayns::Log::debug("Connection closed while receiving data from client {}: '{}'.", id, e.what());
        throw;
    }
    catch (const std::exception &e)
    {
        brayns::Log::error("Unexpected error while receiving data from client {}: '{}'.", id, e.what());
        throw;
    }
    catch (...)
    {
        brayns::Log::error("Unknown error while receiving data from client {}.", id);
        throw;
    }
}

void WebSocket::send(const OutputPacket &packet)
{
    auto id = getId();
    SendLogger::log(packet, id);
    try
    {
        WebSocketSender::send(_socket, packet);
    }
    catch (const brayns::ConnectionClosedException &e)
    {
        brayns::Log::debug("Connection closed while sending data to client {}: '{}'.", id, e.what());
    }
    catch (const std::exception &e)
    {
        brayns::Log::error("Unexpected error while sending data to client {}: '{}'.", id, e.what());
    }
    catch (...)
    {
        brayns::Log::error("Unknown error while sending data client {}.", id);
    }
}
} // namespace brayns
