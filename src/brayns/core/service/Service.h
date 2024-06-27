/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <brayns/core/api/Endpoint.h>
#include <brayns/core/api/Task.h>
#include <brayns/core/utils/Logger.h>
#include <brayns/core/websocket/WebSocketServer.h>

namespace brayns::experimental
{
class StopToken
{
public:
    bool isStopped() const;
    void stop();

private:
    bool _stopped = false;
};

struct ServiceContext
{
    Logger logger;
    WebSocketServerSettings server;
    EndpointRegistry endpoints;
    TaskManager tasks;
    StopToken token = {};
};

class Service
{
public:
    explicit Service(std::unique_ptr<ServiceContext> context);

    void run();

private:
    std::unique_ptr<ServiceContext> _context;
};
}
