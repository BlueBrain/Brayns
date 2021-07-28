/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/common/log.h>

#include "ConnectionHandle.h"
#include "ConnectionManager.h"

namespace brayns
{
class ConnectionRef
{
public:
    ConnectionRef() = default;

    ConnectionRef(ConnectionHandle handle, ConnectionManager& connections)
        : _handle(std::move(handle))
        , _connections(&connections)
    {
    }

    const ConnectionHandle& getHandle() const { return _handle; }

    void send(const OutputPacket& packet) const
    {
        if (!_connections)
        {
            return;
        }
        try
        {
            _connections->send(_handle, packet);
        }
        catch (...)
        {
            BRAYNS_ERROR << "Unexpected error during sending request.\n";
        }
    }

    void broadcast(const OutputPacket& packet) const
    {
        if (!_connections)
        {
            return;
        }
        try
        {
            _connections->broadcast(packet);
        }
        catch (...)
        {
            BRAYNS_ERROR << "Unexpected error during broadcast.\n";
        }
    }

    void broadcastToOtherClients(const OutputPacket& packet) const
    {
        if (!_connections)
        {
            return;
        }
        try
        {
            _connections->broadcast(_handle, packet);
        }
        catch (...)
        {
            BRAYNS_ERROR << "Unexpected error during broadcast to others.\n";
        }
    }

private:
    ConnectionHandle _handle;
    ConnectionManager* _connections = nullptr;
};
} // namespace brayns