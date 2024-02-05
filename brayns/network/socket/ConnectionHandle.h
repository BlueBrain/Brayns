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

#include "NetworkSocket.h"

namespace brayns
{
/**
 * @brief Connection handle used to identify a client.
 *
 * This is basically a shared pointer on a socket instance to make sure it
 * remains unique as long as at least one reference on the client exists in the
 * application (ie not two clients with the same handle).
 *
 */
class ConnectionHandle
{
public:
    /**
     * @brief Construct an invalid handle.
     *
     */
    ConnectionHandle() = default;

    /**
     * @brief Construct a connection handle using the socket.
     *
     * @param socket Client socket.
     */
    ConnectionHandle(NetworkSocketPtr socket)
        : _socket(std::move(socket))
    {
    }

    /**
     * @brief Check if the handle is attached to a client socket.
     *
     * @return true Handle is valid.
     * @return false Handle is invalid.
     */
    bool isValid() const { return bool(_socket); }

    /**
     * @brief Get a unique ID for the handle.
     *
     * @return size_t Unique ID of the handle.
     */
    size_t getId() const { return size_t(_socket.get()); }

    /**
     * @brief Check if other points to the same client.
     *
     * @param other Client handle.
     * @return true Both point to the same client.
     * @return false Other points to a different client.
     */
    bool operator==(const ConnectionHandle& other) const
    {
        return _socket == other._socket;
    }

    /**
     * @brief Unequality shorthand.
     *
     * @param other Client handle.
     * @return true Not the same.
     * @return false Same.
     */
    bool operator!=(const ConnectionHandle& other) const
    {
        return !(*this == other);
    }

private:
    NetworkSocketPtr _socket;
};
} // namespace brayns

namespace std
{
/**
 * @brief Hash connection handle using its ID.
 *
 */
template <>
struct hash<brayns::ConnectionHandle>
{
    size_t operator()(const brayns::ConnectionHandle& handle) const
    {
        return handle.getId();
    }
};
} // namespace std