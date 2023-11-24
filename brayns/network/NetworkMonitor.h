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

#pragma once

#include <condition_variable>
#include <mutex>

#include <brayns/network/client/ClientRef.h>
#include <brayns/network/client/ClientRequest.h>

namespace brayns
{
struct NetworkBuffer
{
    std::vector<ClientRef> connectedClients;
    std::vector<ClientRef> disconnectedClients;
    std::vector<ClientRequest> requests;

    bool isEmpty() const
    {
        return connectedClients.empty() && disconnectedClients.empty() && requests.empty();
    }
};

class NetworkMonitor
{
public:
    NetworkBuffer wait();
    NetworkBuffer poll();
    void clear();
    void notifyConnection(const ClientRef &client);
    void notifyDisonnection(const ClientRef &client);
    void notifyRequest(ClientRequest request);

private:
    std::mutex _mutex;
    std::condition_variable _condition;
    NetworkBuffer _buffer;
};
} // namespace brayns
