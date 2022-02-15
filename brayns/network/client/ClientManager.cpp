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

#include "ClientManager.h"

#include <cassert>

#include <brayns/common/Log.h>

namespace
{
class ClientManagerHelper
{
public:
    static void trySend(const brayns::ClientRef &client, const brayns::OutputPacket &packet)
    {
        try
        {
            auto &socket = client.getSocket();
            socket.send(packet);
        }
        catch (const brayns::ConnectionClosedException &e)
        {
            brayns::Log::debug("Connection closed during broadcast to {}: {}.", client, e.what());
        }
        catch (const std::exception &e)
        {
            brayns::Log::error("Unexpected error during broadcast to {}: {}.", client, e.what());
        }
        catch (...)
        {
            brayns::Log::error("Unknown error during broadcast to {}.", client);
        }
    }
};
} // namespace

namespace brayns
{
ClientManager::~ClientManager()
{
    closeAll();
}

bool ClientManager::isEmpty()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return !_clients.empty();
}

void ClientManager::add(ClientRef client)
{
    assert(client);
    std::lock_guard<std::mutex> lock(_mutex);
    _clients.insert(std::move(client));
}

void ClientManager::remove(const ClientRef &client)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _clients.erase(client);
}

void ClientManager::broadcast(const OutputPacket &packet, const ClientRef &source)
{
    std::lock_guard<std::mutex> lock(_mutex);
    for (const auto &client : _clients)
    {
        if (client == source)
        {
            continue;
        }
        ClientManagerHelper::trySend(client, packet);
    }
}

void ClientManager::closeAll()
{
    std::lock_guard<std::mutex> lock(_mutex);
    for (const auto &client : _clients)
    {
        auto &socket = client.getSocket();
        socket.close();
    }
}
} // namespace brayns
