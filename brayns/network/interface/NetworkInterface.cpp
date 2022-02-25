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

#include <brayns/common/Log.h>

namespace brayns
{
NetworkInterface::NetworkInterface(EntrypointRegistry &entrypoints, ISocket &socket)
    : _entrypoints(entrypoints)
    , _socket(socket)
{
}

void NetworkInterface::add(EntrypointRef entrypoint)
{
    auto &plugin = entrypoint.getPlugin();
    auto &method = entrypoint.getMethod();
    Log::info("Plugin '{}' registered entrypoint '{}'.", plugin, method);
    _entrypoints.add(std::move(entrypoint));
}

void NetworkInterface::poll()
{
    Log::debug("Poll requests from plugin or entrypoint");
    _socket.poll();
}
} // namespace brayns
