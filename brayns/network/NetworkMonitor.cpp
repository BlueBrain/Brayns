/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "NetworkMonitor.h"

#include <utility>

namespace brayns
{
NetworkBuffer NetworkMonitor::wait()
{
    auto lock = std::unique_lock(_mutex);
    if (_buffer.isEmpty())
    {
        _condition.wait(lock);
    }
    return std::exchange(_buffer, {});
}

NetworkBuffer NetworkMonitor::poll()
{
    auto lock = std::lock_guard(_mutex);
    return std::exchange(_buffer, {});
}

void NetworkMonitor::clear()
{
    auto lock = std::lock_guard(_mutex);
    _buffer = {};
}

void NetworkMonitor::notifyConnection(const ClientRef &client)
{
    {
        auto lock = std::lock_guard(_mutex);
        _buffer.connectedClients.push_back(client);
    }
    _condition.notify_all();
}

void NetworkMonitor::notifyDisonnection(const ClientRef &client)
{
    {
        auto lock = std::lock_guard(_mutex);
        _buffer.disconnectedClients.push_back(client);
    }
    _condition.notify_all();
}

void NetworkMonitor::notifyRequest(ClientRequest request)
{
    {
        auto lock = std::lock_guard(_mutex);
        _buffer.requests.push_back(std::move(request));
    }
    _condition.notify_all();
}
} // namespace brayns
