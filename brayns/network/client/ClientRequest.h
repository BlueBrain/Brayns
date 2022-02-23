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

#include <brayns/network/jsonrpc/JsonRpcException.h>
#include <brayns/network/websocket/InputPacket.h>

#include "ClientRef.h"

namespace brayns
{
/**
 * @brief Raw request data with the client that sent it.
 *
 */
class ClientRequest
{
public:
    /**
     * @brief Construct a raw client request.
     *
     * @param client Client sending the data packet.
     * @param packet Packet received from client.
     */
    ClientRequest(ClientRef client, InputPacket packet);

    /**
     * @brief Get the client sending the request.
     *
     * @return const ClientRef& Client ref.
     */
    const ClientRef &getClient() const;

    /**
     * @brief Check if the data received is a binary frame.
     *
     * @return true Binary frame.
     * @return false Something else.
     */
    bool isBinary() const;

    /**
     * @brief Check if the data received is a text frame.
     *
     * @return true Text frame.
     * @return false Something else.
     */
    bool isText() const;

    /**
     * @brief Get the request raw data.
     *
     * @return std::string_view Request data.
     */
    std::string_view getData() const;

    /**
     * @brief Send an error message to the client.
     *
     * Used when the request is not valid JSON-RPC (no ID to reply properly).
     *
     * @param e Error thrown during request parsing.
     */
    void error(const JsonRpcException &e) const;

private:
    ClientRef _client;
    InputPacket _packet;
};
} // namespace brayns
