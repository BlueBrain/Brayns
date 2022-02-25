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

#include "SocketListener.h"

#include <brayns/common/Log.h>

#include <brayns/network/dispatch/RequestDispatcher.h>

namespace
{
class ConnectionHandler
{
public:
    static void handle(
        brayns::ClientManager &clients,
        const brayns::EntrypointRegistry &entrypoints,
        const brayns::ClientRef &client)
    {
        brayns::Log::info("Connection of client {}.", client);
        clients.add(client);
        entrypoints.forEach([&](auto &entrypoint) { entrypoint.onConnect(client); });
    }
};

class DisconnectionHandler
{
public:
    static void handle(
        brayns::ClientManager &clients,
        const brayns::EntrypointRegistry &entrypoints,
        const brayns::ClientRef &client)
    {
        brayns::Log::info("Disconnection of client {}.", client);
        entrypoints.forEach([&](auto &entrypoint) { entrypoint.onDisconnect(client); });
        clients.remove(client);
    }
};

class RequestHandler
{
public:
    static void handle(
        brayns::BinaryManager &binary,
        const brayns::EntrypointRegistry &entrypoints,
        brayns::TaskManager &tasks,
        brayns::ClientRequest request)
    {
        _log(request);
        _dispatch(binary, entrypoints, tasks, std::move(request));
    }

private:
    static void _log(const brayns::ClientRequest &request)
    {
        brayns::Log::info("Received request {}.", request);
        auto data = request.isBinary() ? "<Binary data>" : request.getData();
        brayns::Log::debug("Request content: '{}'.", data);
    }

    static void _dispatch(
        brayns::BinaryManager &binary,
        const brayns::EntrypointRegistry &entrypoints,
        brayns::TaskManager &tasks,
        brayns::ClientRequest request)
    {
        brayns::RequestDispatcher dispatcher(binary, entrypoints, tasks);
        dispatcher.dispatch(std::move(request));
    }
};
} // namespace

namespace brayns
{
SocketListener::SocketListener(
    BinaryManager &binary,
    ClientManager &clients,
    const EntrypointRegistry &entrypoints,
    TaskManager &tasks)
    : _binary(binary)
    , _clients(clients)
    , _entrypoints(entrypoints)
    , _tasks(tasks)
{
}

void SocketListener::onConnect(const ClientRef &client)
{
    ConnectionHandler::handle(_clients, _entrypoints, client);
}

void SocketListener::onDisconnect(const ClientRef &client)
{
    DisconnectionHandler::handle(_clients, _entrypoints, client);
}

void SocketListener::onRequest(ClientRequest request)
{
    RequestHandler::handle(_binary, _entrypoints, _tasks, std::move(request));
}
} // namespace brayns
