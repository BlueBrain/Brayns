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

#include <unordered_map>

#include "Connection.h"
#include "ConnectionHandle.h"

namespace brayns
{
/**
 * @brief Connection info indexed by client handle.
 *
 */
class ConnectionMap
{
public:
    size_t getConnectionCount() const { return _connections.size(); }

    Connection* find(const ConnectionHandle& handle)
    {
        auto connection = _find(handle);
        if (!connection || connection->removed)
        {
            return nullptr;
        }
        return connection;
    }

    void add(NetworkSocketPtr socket)
    {
        auto& connection = _connections[socket];
        connection.socket = std::move(socket);
    }

    void markAsRemoved(const ConnectionHandle& handle)
    {
        auto connection = _find(handle);
        if (!connection)
        {
            return;
        }
        connection->removed = true;
    }

    template <typename FunctorType>
    void forEach(FunctorType functor) const
    {
        for (const auto& pair : _connections)
        {
            auto& handle = pair.first;
            auto& connection = pair.second;
            functor(handle, connection);
        }
    }

    template <typename FunctorType>
    void removeIf(FunctorType functor)
    {
        for (auto i = _connections.begin(); i != _connections.end();)
        {
            auto& handle = i->first;
            auto& connection = i->second;
            if (functor(handle, connection))
            {
                i = _connections.erase(i);
                continue;
            }
            ++i;
        }
    }

private:
    Connection* _find(const ConnectionHandle& handle)
    {
        auto i = _connections.find(handle);
        return i == _connections.end() ? nullptr : &i->second;
    }

    std::unordered_map<ConnectionHandle, Connection> _connections;
};
} // namespace brayns