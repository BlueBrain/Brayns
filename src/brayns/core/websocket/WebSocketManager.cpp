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

#include "WebSocketManager.h"

#include <string>
#include <utility>

namespace
{
using namespace brayns::experimental;
using namespace brayns;

struct WebSocketBuffer
{
    bool binary = false;
    std::string data;
};

void onContinuation(const WebSocketFrame &frame, WebSocketBuffer &buffer, Logger &logger)
{
    logger.info("Continuation frame received (binary = {})", buffer.binary);

    if (buffer.data.empty())
    {
        throw WebSocketException(WebSocketStatus::PayloadNotAcceptable, "Continuation frame outside fragmented message");
    }

    buffer.data.append(frame.data);
}

void onText(const WebSocketFrame &frame, WebSocketBuffer &buffer, Logger &logger)
{
    logger.info("Text frame received");

    if (!buffer.data.empty())
    {
        throw WebSocketException(WebSocketStatus::PayloadNotAcceptable, "Incomplete fragmented frame");
    }

    buffer.data.append(frame.data);
    buffer.binary = false;
}

void onBinary(const WebSocketFrame &frame, WebSocketBuffer &buffer, Logger &logger)
{
    logger.info("Binary frame received");

    if (!buffer.data.empty())
    {
        throw WebSocketException(WebSocketStatus::PayloadNotAcceptable, "Incomplete fragmented frame");
    }

    buffer.data.append(frame.data);
    buffer.binary = true;
}

void onClose(const WebSocketConnection &websocket, Logger &logger)
{
    logger.info("Close frame received, sending normal close frame");

    websocket.closeOk();
}

void onPing(const WebSocketConnection &websocket, Logger &logger)
{
    logger.info("Ping frame received, sending pong frame");

    websocket.send({.opcode = WebSocketOpcode::Pong});
}

void onPong(Logger &logger)
{
    logger.info("Pong frame received, ignoring");
}

void respond(const WebSocketConnection &websocket, const RawResponse &response)
{
    websocket.send({
        .opcode = response.binary ? WebSocketOpcode::Binary : WebSocketOpcode::Text,
        .data = response.data,
    });
}

void runClientLoop(
    ClientId clientId,
    const WebSocketConnection &websocket,
    const WebSocketListener &listener,
    Logger &logger)
{
    auto buffer = WebSocketBuffer();

    while (true)
    {
        auto frame = websocket.receive();

        switch (frame.opcode)
        {
        case WebSocketOpcode::Continuation:
            onContinuation(frame, buffer, logger);
            break;
        case WebSocketOpcode::Text:
            onText(frame, buffer, logger);
            break;
        case WebSocketOpcode::Binary:
            onBinary(frame, buffer, logger);
            break;
        case WebSocketOpcode::Close:
            onClose(websocket, logger);
            return;
        case WebSocketOpcode::Ping:
            onPing(websocket, logger);
            continue;
        case WebSocketOpcode::Pong:
            onPong(logger);
            continue;
        default:
            throw WebSocketException(WebSocketStatus::UnexpectedCondition, "Unexpected invalid opcode");
        }

        if (!frame.finalFrame)
        {
            continue;
        }

        listener.onRequest({
            .clientId = clientId,
            .data = std::exchange(buffer.data, {}),
            .binary = buffer.binary,
            .respond = [=](const auto &response) { respond(websocket, response); },
        });
    }
}
}

namespace brayns::experimental
{
WebSocketManager::WebSocketManager(WebSocketListener listener, Logger &logger):
    _listener(std::move(listener)),
    _logger(&logger)
{
}

void WebSocketManager::handle(const WebSocketConnection &websocket)
{
    auto clientId = _clientIds.next();

    try
    {
        runClientLoop(clientId, websocket, _listener, *_logger);
    }
    catch (...)
    {
        _logger->error("Unexpected error in websocket client loop");
    }

    _clientIds.recycle(clientId);
}
}
