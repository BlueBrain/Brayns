/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/WebSocket.h>
#include <Poco/URI.h>

namespace brayns
{
class InputPacket
{
public:
    InputPacket() = default;

    InputPacket(const Poco::Buffer<char>& buffer, int flags)
        : _data(buffer.begin(), buffer.size())
        , _flags(flags)
    {
    }

    bool isEmpty() const { return _data.empty(); }

    const std::string& getData() const { return _data; }

    bool isBinary() const
    {
        return _flags & Poco::Net::WebSocket::FRAME_OP_BINARY;
    }

    bool isText() const { return _flags & Poco::Net::WebSocket::FRAME_OP_TEXT; }

    bool isClose() const
    {
        return _flags & Poco::Net::WebSocket::FRAME_OP_CLOSE;
    }

private:
    std::string _data;
    int _flags = 0;
};

class OutputPacket
{
public:
    OutputPacket() = default;

    OutputPacket(const std::string& data)
        : _data(data.data())
        , _size(int(data.size()))
        , _flags(Poco::Net::WebSocket::FRAME_TEXT)
    {
    }

    OutputPacket(const void* data, int size)
        : _data(data)
        , _size(size)
        , _flags(Poco::Net::WebSocket::FRAME_BINARY)
    {
    }

    bool isEmpty() const { return _size <= 0; }

    const void* getData() const { return _data; }

    int getSize() const { return _size; }

    int getFlags() const { return _flags; }

private:
    const void* _data;
    int _size = 0;
    int _flags = 0;
};

class ConnectionClosedException : public std::runtime_error
{
public:
    ConnectionClosedException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

class NetworkSocket
{
public:
    NetworkSocket(Poco::Net::HTTPServerRequest& request,
                  Poco::Net::HTTPServerResponse& response)
        : _socket(request, response)
    {
        _protectAgainstMemoryExhaustion();
    }

    InputPacket receive()
    {
        Poco::Buffer<char> buffer(0);
        int flags = 0;
        int size = 0;
        try
        {
            size = _socket.receiveFrame(buffer, flags);
        }
        catch (Poco::Exception& e)
        {
            throw ConnectionClosedException(e.displayText());
        }
        if (size <= 0)
        {
            throw ConnectionClosedException("Failed to receive frame");
        }
        return {buffer, flags};
    }

    void send(const OutputPacket& frame)
    {
        int size = 0;
        try
        {
            size = _socket.sendFrame(frame.getData(), frame.getSize(),
                                     frame.getFlags());
        }
        catch (Poco::Exception& e)
        {
            throw ConnectionClosedException(e.displayText());
        }
        if (size < frame.getSize())
        {
            throw ConnectionClosedException("Failed to send frame");
        }
    }

private:
    void _protectAgainstMemoryExhaustion()
    {
        _socket.setMaxPayloadSize(int(1e6));
    }

    Poco::Net::WebSocket _socket;
};
} // namespace brayns