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

#pragma once

#include <stdexcept>
#include <string_view>

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/WebSocket.h>

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
     * @param data Raw data of the packet.
     * @param flags Packet info (binary, close, etc).
     */
    InputPacket(Poco::Buffer<char> data, int flags);

    /**
     * @brief Check if the packet data is empty (no flags, no data).
     *
     * @return true No data inside the packet.
     * @return false The packet is not empty.
     */
    bool isEmpty() const;

    /**
     * @brief Get the data of the packet.
     *
     * @return std::string_view Raw data of the packet.
     */
    std::string_view getData() const;

    /**
     * @brief Check if the packet content is in binary format.
     *
     * @return true The packet is a binary packet.
     * @return false The packet is not a binary packet.
     */
    bool isBinary() const;
    /**
     * @brief Check if the packet content is in text format.
     *
     * @return true The packet is a text packet.
     * @return false The packet is not a text packet.
     */
    bool isText() const;

    /**
     * @brief Check if the packet is a close packet.
     *
     * @return true The client want to close the connection.
     * @return false The packet is not a close packet.
     */
    bool isClose() const;

private:
    Poco::Buffer<char> _data = 0;
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
     * @brief Construct a text packet.
     *
     * @param data The text content of the packet.
     */
    static OutputPacket fromText(std::string_view data);

    /**
     * @brief Construct a binary packet.
     *
     * @param data The binary content of the packet.
     */
    static OutputPacket fromBinary(std::string_view data);

    /**
     * @brief Construct an empty packet.
     *
     */
    OutputPacket() = default;

    /**
     * @brief Construct a packet with all information.
     *
     * @param data Pointer to packet data.
     * @param size Packet size.
     * @param flags Flags (text or binary).
     */
    OutputPacket(const void *data, size_t size, int flags);

    /**
     * @brief Check if the packet is empty.
     *
     * @return true The packet is empty.
     * @return false The packet is not empty.
     */
    bool isEmpty() const;

    /**
     * @brief Get the data to send.
     *
     * @return const void* Data to send.
     */
    const void *getData() const;

    /**
     * @brief Get the size of the packet content.
     *
     * @return int The size of the packet content.
     */
    int getSize() const;

    /**
     * @brief Get the raw flags of the packet.
     *
     * @return int Raw flags containing packet info.
     */
    int getFlags() const;

private:
    const void *_data;
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
    ConnectionClosedException(const std::string &message);
};

/**
 * @brief WebSocket used to send an receive data.
 *
 */
class WebSocket
{
public:
    /**
     * @brief Construct a client side websocket from client session.
     *
     * @param session Client session.
     * @param request Client request.
     * @param response Server response.
     */
    WebSocket(
        Poco::Net::HTTPClientSession &session,
        Poco::Net::HTTPRequest &request,
        Poco::Net::HTTPResponse &response);

    /**
     * @brief Construct a server side websocket from client HTTP request.
     *
     * @param request Client request.
     * @param response Server response.
     */
    WebSocket(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);

    /**
     * @brief Close the socket.
     *
     */
    void close();

    /**
     * @brief Receive an input packet from the connected client.
     *
     * Block until data is received.
     *
     * @return InputPacket Data packet received from the client (always valid).
     * @throw ConnectionClosedException The client closed the connection.
     */
    InputPacket receive();

    /**
     * @brief Send an output packet to the connected client.
     *
     * @param packet Packet containing the data to send to the client.
     */
    void send(const OutputPacket &packet);

private:
    Poco::Net::WebSocket _socket;
};
} // namespace brayns
