/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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
    size_t getConnectionCount() const;
    Connection *find(const ConnectionHandle &handle);
    void add(NetworkSocketPtr socket);
    void markAsRemoved(const ConnectionHandle &handle);

    template<typename FunctorType>
    void forEach(FunctorType functor) const
    {
        for (const auto &pair : _connections)
        {
            auto &handle = pair.first;
            auto &connection = pair.second;
            functor(handle, connection);
        }
    }

    template<typename FunctorType>
    void removeIf(FunctorType functor)
    {
        for (auto i = _connections.begin(); i != _connections.end();)
        {
            auto &handle = i->first;
            auto &connection = i->second;
            if (functor(handle, connection))
            {
                i = _connections.erase(i);
                continue;
            }
            ++i;
        }
    }

private:
    Connection *_find(const ConnectionHandle &handle);

    std::unordered_map<ConnectionHandle, Connection> _connections;
};
} // namespace brayns
