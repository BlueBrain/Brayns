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

#include <functional>
#include <memory>

#include <spdlog/fmt/fmt.h>

#include <brayns/network/websocket/IWebSocket.h>

namespace brayns
{
/**
 * @brief Ref counted object to access a connected client and its socket.
 *
 */
class ClientRef
{
public:
    /**
     * @brief Construct a connected client.
     *
     * @param socket Client socket (cannot be null).
     */
    explicit ClientRef(std::shared_ptr<IWebSocket> socket);

    /**
     * @brief Get a unique id for the client determined by the socket.
     *
     * @return size_t Client ID.
     */
    size_t getId() const;

    /**
     * @brief Close the client socket.
     *
     */
    void disconnect() const;

    /**
     * @brief Block until a data packet is received and return it.
     *
     * @return InputPacket Packet received from the client.
     */
    InputPacket receive() const;

    /**
     * @brief Send data packet to client.
     *
     * @param packet Packet to send to the client.
     */
    void send(const OutputPacket &packet) const;

    /**
     * @brief Comparison using underlying socket.
     *
     * @param other RHS.
     * @return true Same client.
     * @return false Different client.
     */
    bool operator==(const ClientRef &other) const;

    /**
     * @brief Comparison using underlying socket.
     *
     * @param other RHS.
     * @return true Different client.
     * @return false Same client.
     */
    bool operator!=(const ClientRef &other) const;

private:
    std::shared_ptr<IWebSocket> _socket;
};
} // namespace brayns

namespace std
{
template<>
struct hash<brayns::ClientRef>
{
    size_t operator()(const brayns::ClientRef &client) const
    {
        return client.getId();
    }
};
} // namespace std

namespace fmt
{
template<>
struct formatter<brayns::ClientRef> : fmt::formatter<size_t>
{
    auto format(const brayns::ClientRef &client, fmt::format_context &context) const
    {
        return fmt::formatter<size_t>::format(client.getId(), context);
    }
};
} // namespace fmt
