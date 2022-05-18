/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "SocketManager.h"

#include <cassert>
#include <optional>

#include <brayns/common/Log.h>

namespace
{
class MessageReceiver
{
public:
    static std::optional<brayns::InputPacket> tryReceive(const brayns::ClientRef &client)
    {
        try
        {
            auto &socket = client.getSocket();
            return socket.receive();
        }
        catch (const brayns::ConnectionClosedException &e)
        {
            brayns::Log::debug("Connection closed during reception from client {}: '{}'.", client, e.what());
        }
        catch (const std::exception &e)
        {
            brayns::Log::error("Unexpected error during reception from client {}: '{}'.", client, e.what());
        }
        catch (...)
        {
            brayns::Log::error("Unexpected error during reception from client {}.", client);
        }
        return std::nullopt;
    }
};
} // namespace

namespace brayns
{
SocketManager::SocketManager(std::unique_ptr<ISocketListener> listener)
    : _listener(std::move(listener))
{
    assert(_listener);
}

void SocketManager::run(const ClientRef &client)
{
    _newClients.add(client);
    while (true)
    {
        auto packet = MessageReceiver::tryReceive(client);
        if (!packet)
        {
            break;
        }
        _requests.add({client, std::move(*packet)});
    }
    _removedClients.add(client);
}

void SocketManager::poll()
{
    for (const auto &client : _newClients.poll())
    {
        _listener->onConnect(client);
    }
    for (auto &request : _requests.poll())
    {
        _listener->onRequest(std::move(request));
    }
    for (const auto &client : _removedClients.poll())
    {
        _listener->onDisconnect(client);
    }
}
} // namespace brayns
