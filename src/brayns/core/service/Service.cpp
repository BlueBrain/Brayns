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

#include "Service.h"

#include "RequestHandler.h"

namespace brayns
{
bool StopToken::isStopped() const
{
    return _stopped;
}

void StopToken::stop()
{
    _stopped = true;
}

Service::Service(std::unique_ptr<ServiceContext> context):
    _context(std::move(context))
{
}

void Service::run()
{
    auto handler = RequestHandler(_context->endpoints, _context->tasks, _context->logger);

    auto server = startServer(_context->server, _context->logger);

    while (true)
    {
        auto requests = server.waitForRequests();

        for (const auto &request : requests)
        {
            handler.handle(request);

            if (_context->token.isStopped())
            {
                return;
            }
        }
    }
}
}
