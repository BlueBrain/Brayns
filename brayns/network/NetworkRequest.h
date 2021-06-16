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

#include <string>

#include <Poco/Net/WebSocket.h>

namespace brayns
{
/**
 * @brief Wrapper around a WebSocket to send and receive text frames.
 * 
 */
class NetworkRequest
{
public:
    /**
     * @brief Underlying socket.
     * 
     */
    using Socket = Poco::Net::WebSocket;

    /**
     * @brief Construct a network request to a given entrypoint.
     * 
     * A high max payload will be setup to avoid DoS attacks.
     * 
     * @param name Name of the entrypoint the request addresses.
     * @param socket The socket to use for message exchange.
     */
    NetworkRequest(std::string name, Socket& socket)
        : _name(std::move(name))
        , _socket(&socket)
    {
        _protectAgainstMemoryExhaustion();
    }

    /**
     * @brief Get the name of the entrypoint the request addresses.
     * 
     * @return const std::string& Name of the entrypoint.
     */
    const std::string& getName() const { return _name; }

    /**
     * @brief Receive a text frame from the underlying socket.
     * 
     * @return std::string Raw text frame.
     * @throw std::runtime_error The frame cannot be received or is not text.
     */
    std::string receive()
    {
        Poco::Buffer<char> buffer(1024);
        int flags = 0;
        auto size = _socket->receiveFrame(buffer, flags);
        if (size <= 0)
        {
            throw std::runtime_error("Failed to receive frame");
        }
        if (!(flags & Socket::FRAME_OP_TEXT))
        {
            throw std::runtime_error("Not a text frame");
        }
        return buffer.begin();
    }

    /**
     * @brief Send a text frame through the underlying socket.
     * 
     * @param data Raw text frame.
     * @throw std::runtime_error Cannot send the frame.
     */
    void send(const std::string& data)
    {
        if (data.empty())
        {
            return;
        }
        auto size = _socket->sendFrame(data.data(), int(data.size()));
        if (size <= 0)
        {
            throw std::runtime_error("Failed to send frame");
        }
    }

private:
    void _protectAgainstMemoryExhaustion()
    {
        _socket->setMaxPayloadSize(int(1e6));
    }

    std::string _name;
    Socket* _socket;
};
} // namespace brayns