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

#include "WebSocketHandler.h"

#include <string>
#include <utility>

#include <brayns/core/utils/Binary.h>

namespace
{
using namespace brayns;

void onContinuation(const WebSocketFrame &frame, Message &buffer, Logger &logger)
{
    logger.info("Continuation frame received (binary = {})", buffer.binary);

    if (buffer.data.empty())
    {
        throw WebSocketException(WebSocketStatus::PayloadNotAcceptable, "Continuation frame outside fragmented message");
    }

    buffer.data.append(frame.data);
}

void onText(const WebSocketFrame &frame, Message &buffer, Logger &logger)
{
    logger.info("Text frame received");

    if (!buffer.data.empty())
    {
        throw WebSocketException(WebSocketStatus::PayloadNotAcceptable, "Incomplete text frame");
    }

    buffer.data.append(frame.data);
    buffer.binary = false;
}

void onBinary(const WebSocketFrame &frame, Message &buffer, Logger &logger)
{
    logger.info("Binary frame received");

    if (!buffer.data.empty())
    {
        throw WebSocketException(WebSocketStatus::PayloadNotAcceptable, "Incomplete binary frame");
    }

    buffer.data.append(frame.data);
    buffer.binary = true;
}

void onClose(WebSocket &websocket, Logger &logger)
{
    logger.info("Close frame received, sending normal close frame");

    websocket.close(WebSocketStatus::NormalClose);
}

void onPing(const WebSocketFrame &frame, WebSocket &websocket, Logger &logger)
{
    logger.info("Ping frame received, sending pong frame");

    websocket.send({.opcode = WebSocketOpcode::Pong, .data = frame.data});
}

void onPong(Logger &logger)
{
    logger.info("Pong frame received, ignoring");
}

void respond(ClientId clientId, WebSocket &websocket, Logger &logger, const Message &response)
{
    auto data = std::string_view(response.data);

    logger.info("Sending response of {} bytes to client {}", data.size(), clientId);

    if (!response.binary)
    {
        logger.debug("Text response data: {}", data);
    }

    auto opcode = response.binary ? WebSocketOpcode::Binary : WebSocketOpcode::Text;

    auto maxFrameSize = websocket.getMaxFrameSize();

    while (true)
    {
        auto chunk = extractBytes(data, maxFrameSize);
        auto finalFrame = data.empty();

        try
        {
            logger.info("Sending websocket frame of {} bytes", data.size());
            websocket.send({opcode, chunk, finalFrame});
        }
        catch (const WebSocketException &e)
        {
            logger.warn("Failed to send websocket frame: {}", e.what());
            websocket.close(e.getStatus(), e.what());
            return;
        }
        catch (...)
        {
            logger.error("Unexpected error while sending websocket frame");
            websocket.close(WebSocketStatus::UnexpectedCondition, "Internal error");
            return;
        }

        if (finalFrame)
        {
            logger.info("Successfully sent websocket response frame(s)");
            return;
        }
    }
}

void runClientLoop(ClientId clientId, WebSocket &websocket, RequestQueue &requests, Logger &logger)
{
    auto buffer = Message();

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
            onPing(frame, websocket, logger);
            continue;
        case WebSocketOpcode::Pong:
            onPong(logger);
            continue;
        default:
            logger.error("Unexpected invalid opcode: {}", static_cast<int>(frame.opcode));
            throw WebSocketException(WebSocketStatus::UnexpectedCondition, "Unexpected invalid opcode");
        }

        if (!frame.finalFrame)
        {
            continue;
        }

        logger.info("Received request of {} bytes from client {}", buffer.data.size(), clientId);

        if (!buffer.binary)
        {
            logger.debug("Text request data: {}", buffer.data);
        }

        auto request = Request{
            .clientId = clientId,
            .message = std::exchange(buffer, {}),
            .respond = [=, &logger](const auto &response) mutable { respond(clientId, websocket, logger, response); },
        };

        requests.push(std::move(request));
    }
}

ClientId generateClientId(std::mutex &mutex, IdGenerator<ClientId> &ids)
{
    auto lock = std::lock_guard(mutex);
    return ids.next();
}

void recycleClientId(std::mutex &mutex, IdGenerator<ClientId> &ids, ClientId id)
{
    auto lock = std::lock_guard(mutex);
    return ids.recycle(id);
}
}

namespace brayns
{
WebSocketHandler::WebSocketHandler(RequestQueue &requests, Logger &logger):
    _requests(&requests),
    _logger(&logger)
{
}

void WebSocketHandler::handle(WebSocket &websocket)
{
    auto clientId = generateClientId(_mutex, _ids);

    try
    {
        _logger->info("New client connected with ID {}", clientId);
        runClientLoop(clientId, websocket, *_requests, *_logger);
    }
    catch (const WebSocketClosed &e)
    {
        _logger->warn("WebSocket closed by peer: {}", e.what());
    }
    catch (const WebSocketException &e)
    {
        _logger->warn("Error while processing websocket: '{}'", e.what());
        websocket.close(e.getStatus(), e.what());
    }
    catch (...)
    {
        _logger->error("Unknown error in websocket handler");
        websocket.close(WebSocketStatus::UnexpectedCondition, "Internal error");
    }

    recycleClientId(_mutex, _ids, clientId);

    _logger->info("Client {} disconnected", clientId);
}
}
