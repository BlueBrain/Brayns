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

#include <functional>
#include <memory>

#include <spdlog/fmt/ostr.h>

#include <brayns/network/websocket/WebSocket.h>

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
     * @param socket Client socket.
     */
    ClientRef(std::shared_ptr<WebSocket> socket);

    /**
     * @brief Get client socket (must be connected).
     *
     * @return WebSocket& Client socket.
     */
    WebSocket &getSocket() const;

    /**
     * @brief Get a unique id for the client determined by the socket.
     *
     * @return size_t Client ID.
     */
    size_t getId() const;

    /**
     * @brief Comparison using underlying socket.
     *
     * @param other LHS.
     * @return true Same client.
     * @return false Different client.
     */
    bool operator==(const ClientRef &other) const;

    /**
     * @brief Comparison using underlying socket.
     *
     * @param other LHS.
     * @return true Different client.
     * @return false Same client.
     */
    bool operator!=(const ClientRef &other) const;

private:
    std::shared_ptr<WebSocket> _socket;
};
} // namespace brayns

namespace std
{
std::ostream &operator<<(std::ostream &stream, const brayns::ClientRef &client);

template<>
struct hash<brayns::ClientRef>
{
    size_t operator()(const brayns::ClientRef &client) const;
};
} // namespace std
