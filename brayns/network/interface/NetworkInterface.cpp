/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 *
 * Responsible Authors: Daniel.Nachbaur@epfl.ch
 *                      Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software{} you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY{} without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library{} if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "NetworkInterface.h"

#include <brayns/network/jsonrpc/JsonRpcSender.h>

namespace brayns
{
NetworkInterface::NetworkInterface(EntrypointManager &entrypoints, NetworkTaskManager &tasks, ClientManager &clients)
    : _entrypoints(entrypoints)
    , _tasks(tasks)
    , _clients(clients)
{
}

void NetworkInterface::addEntrypoint(EntrypointRef entrypoint)
{
    _entrypoints.add(std::move(entrypoint));
}

void NetworkInterface::addTask(const ClientRef &client, const RequestId &id, std::unique_ptr<NetworkTask> task)
{
    _tasks.add(client, id, std::move(task));
}

void NetworkInterface::notify(const NotificationMessage &message)
{
    JsonRpcSender::notification(message, _clients);
}
} // namespace brayns
