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

#include <mutex>
#include <unordered_map>

#include "ClientRef.h"

namespace brayns
{
/**
 * @brief Synchronized buffer to store incoming requests per client.
 *
 */
class RequestBuffer
{
public:
    /**
     * @brief Client -> list of requests.
     *
     */
    using Map = std::unordered_map<ClientRef, std::vector<InputPacket>>;

    /**
     * @brief Add a request received from the given client.
     *
     * @param client Client sending packet.
     * @param packet Packet sent by client.
     */
    void add(const ClientRef &client, InputPacket packet);

    /**
     * @brief Get all requests received since last call.
     *
     * @return Map Mapping from client to list of requests.
     */
    Map extractAll();

private:
    std::mutex _mutex;
    Map _buffers;
};
} // namespace brayns
