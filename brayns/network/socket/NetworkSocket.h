/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <memory>
#include <stdexcept>
#include <string>

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/WebSocket.h>
#include <Poco/URI.h>

namespace brayns
{
/**
 * @brief Input data packet received from a WebSocket.
 *
 */
class InputPacket
{
public:
    /**
     * @brief Construct an empty packet (no flags, no data).
     *
     */
    InputPacket() = default;

    /**
     * @brief Construct a packet using the buffer data and the flags.
     *
     * @param buffer Raw data of the packet.
     * @param flags Packet info (binary, close, etc).
     */
    InputPacket(const Poco::Buffer<char>& buffer, int flags)
        : _data(buffer.begin(), buffer.size())
        , _flags(flags)
    {
    }

    /**
     * @brief Check if the packet data is empty (no flags, no data).
     *
     * @return true No data inside the packet.
     * @return false The packet is not empty.
     */
    bool isEmpty() const { return _flags == 0 && _data.empty(); }

    /**
     * @brief Get the data of the packet.
     *
     * @return const std::string& Raw data of the packet.
     */
    const std::string& getData() const { return _data; }

    /**
     * @brief Check if the packet content is in binary format.
     *
     * @return true The packet is a binary packet.
     * @return false The packet is not a binary packet.
     */
    bool isBinary() const
    {
        return _flags & Poco::Net::WebSocket::FRAME_OP_BINARY;
    }

    /**
     * @brief Check if the packet content is in text format.
     *
     * @return true The packet is a text packet.
     * @return false The packet is not a text packet.
     */
    bool isText() const { return _flags & Poco::Net::WebSocket::FRAME_OP_TEXT; }

    /**
     * @brief Check if the packet is a close packet.
     *
     * @return true The client want to close the connection.
     * @return false The packet is not a close packet.
     */
    bool isClose() const
    {
        return _flags & Poco::Net::WebSocket::FRAME_OP_CLOSE;
    }

private:
    std::string _data;
    int _flags = 0;
};

/**
 * @brief Output data packet to send data through a WebSocket.
 *
 * Don't own the packet content.
 *
 */
class OutputPacket
{
public:
    /**
     * @brief Construct an empty packet.
     *
     */
    OutputPacket() = default;

    /**
     * @brief Construct a text packet.
     *
     * @param data The text content of the packet.
     */
    OutputPacket(const std::string& data)
        : _data(data.data())
        , _size(int(data.size()))
        , _flags(Poco::Net::WebSocket::FRAME_TEXT)
    {
    }

    /**
     * @brief Construct a binary packet.
     *
     * @param data The binary content of the packet.
     * @param size The size of the packet content.
     */
    OutputPacket(const void* data, int size)
        : _data(data)
        , _size(size)
        , _flags(Poco::Net::WebSocket::FRAME_BINARY)
    {
    }

    /**
     * @brief Check if the packet is empty.
     *
     * @return true The packet is empty.
     * @return false The packet is not empty.
     */
    bool isEmpty() const { return _size <= 0; }

    /**
     * @brief Get the data to send.
     *
     * @return const void* Data to send.
     */
    const void* getData() const { return _data; }

    /**
     * @brief Get the size of the packet content.
     *
     * @return int The size of the packet content.
     */
    int getSize() const { return _size; }

    /**
     * @brief Get the raw flags of the packet.
     *
     * @return int Raw flags containing packet info.
     */
    int getFlags() const { return _flags; }

private:
    const void* _data;
    int _size = 0;
    int _flags = 0;
};

/**
 * @brief Exception thrown by the socket when an operation cannot be achieved
 * because the connection is closed.
 *
 */
class ConnectionClosedException : public std::runtime_error
{
public:
    /**
     * @brief Construct an exception instance.
     *
     * @param message Description of the reason why the connection was closed.
     */
    ConnectionClosedException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

/**
 * @brief WebSocket used to send an receive data.
 *
 */
class NetworkSocket
{
public:
    /**
     * @brief Construct a client side websocket from client session.
     *
     * @param session Client session.
     * @param request Client request.
     * @param response Server response.
     */
    NetworkSocket(Poco::Net::HTTPClientSession& session,
                  Poco::Net::HTTPRequest& request,
                  Poco::Net::HTTPResponse& response)
        : _socket(session, request, response)
    {
        _setupSocket();
    }

    /**
     * @brief Construct a server side websocket from client HTTP request.
     *
     * @param request Client request.
     * @param response Server response.
     */
    NetworkSocket(Poco::Net::HTTPServerRequest& request,
                  Poco::Net::HTTPServerResponse& response)
        : _socket(request, response)
    {
        _setupSocket();
    }

    /**
     * @brief Close the socket.
     *
     */
    void close() { _socket.close(); }

    /**
     * @brief Receive an input packet from the connected client.
     *
     * Block until data is received.
     *
     * @return InputPacket Data packet received from the client (always valid).
     * @throw ConnectionClosedException The client closed the connection.
     */
    InputPacket receive()
    {
        Poco::Buffer<char> buffer(0);
        int flags = 0;
        try
        {
            _socket.receiveFrame(buffer, flags);
        }
        catch (Poco::Exception& e)
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

    /**
     * @brief Send an output packet to the connected client.
     *
     * @param packet Packet containing the data to send to the client.
     */
    void send(const OutputPacket& packet)
    {
        int size = 0;
        try
        {
            size = _socket.sendFrame(packet.getData(), packet.getSize(),
                                     packet.getFlags());
        }
        catch (Poco::Exception& e)
        {
            throw ConnectionClosedException(e.displayText());
        }
        if (size < packet.getSize())
        {
            throw ConnectionClosedException("Cannot send frame entirely");
        }
    }

private:
    void _setupSocket()
    {
        _socket.setReceiveTimeout(Poco::Timespan());
        _socket.setSendTimeout(Poco::Timespan());
    }

    Poco::Net::WebSocket _socket;
};

using NetworkSocketPtr = std::shared_ptr<NetworkSocket>;
} // namespace brayns