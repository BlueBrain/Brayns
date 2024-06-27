/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <doctest.h>

#include <condition_variable>
#include <mutex>
#include <thread>

#include <brayns/core/websocket/WebSocketHandler.h>
#include <brayns/core/websocket/WebSocketServer.h>

using namespace brayns;

TEST_CASE("WebSocket")
{
    SUBCASE("Server")
    {
        /*auto logger = createConsoleLogger("Test");
        logger.setLevel(LogLevel::Debug);

        auto mutex = std::mutex();
        auto condition = std::condition_variable();

        auto onConnect = [&](auto clientId) { logger.info("Client {} connected", clientId); };

        auto onDisconnect = [&](auto clientId)
        {
            logger.info("Client {} disconnected", clientId);
            auto lock = std::lock_guard(mutex);
            condition.notify_all();
        };

        auto onRequest = [&](const auto &request)
        {
            logger.info("Request of {} bytes", request.data.size());
            // request.respond({request.data, request.binary});
        };

        auto listener = WebSocketListener{onConnect, onDisconnect, onRequest};

        auto handler = WebSocketHandler(listener, logger);

        auto settings = WebSocketServerSettings{};

        auto server = startWebSocketServer(settings, handler, logger);

        auto lock = std::unique_lock(mutex);
        condition.wait(lock);*/
    }
}
