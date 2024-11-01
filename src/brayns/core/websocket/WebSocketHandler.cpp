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

#include <mutex>
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

std::optional<Message> receiveRequest(Logger &logger, ClientId client, WebSocket &websocket)
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
            return std::nullopt;
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

        logger.info("Received request of {} bytes from client {}", buffer.data.size(), client);

        if (!buffer.binary)
        {
            logger.debug("Text request data: {}", buffer.data);
        }

        return buffer;
    }
}

void sendResponse(Logger &logger, ClientId client, WebSocket &websocket, const Message &message)
{
    auto data = std::string_view(message.data);

    logger.info("Sending response of {} bytes to client {}", data.size(), client);

    if (!message.binary)
    {
        logger.debug("Text response data: {}", data);
    }

    auto opcode = message.binary ? WebSocketOpcode::Binary : WebSocketOpcode::Text;

    auto maxFrameSize = websocket.getMaxFrameSize();

    while (true)
    {
        auto chunk = extractBytes(data, maxFrameSize);
        auto finalFrame = data.empty();

        try
        {
            logger.info("Sending websocket frame of {} bytes", chunk.size());
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

class WebSocketWrapper
{
public:
    explicit WebSocketWrapper(Logger &logger, ClientId client, WebSocket websocket):
        _logger(&logger),
        _client(client),
        _websocket(std::move(websocket))
    {
    }

    void close(WebSocketStatus status, std::string_view message = {})
    {
        _websocket.close(status, message);
    }

    ClientId getClient() const
    {
        return _client;
    }

    std::optional<Message> receive()
    {
        return receiveRequest(*_logger, _client, _websocket);
    }

    void send(const Message &message)
    {
        auto lock = std::lock_guard(_mutex);
        sendResponse(*_logger, _client, _websocket, message);
    }

private:
    std::mutex _mutex;
    Logger *_logger;
    ClientId _client;
    WebSocket _websocket;
};

void runClientLoop(RequestQueue &requests, std::shared_ptr<WebSocketWrapper> websocket)
{
    while (true)
    {
        auto message = websocket->receive();

        if (!message)
        {
            return;
        }

        auto request = Request{
            .client = websocket->getClient(),
            .message = std::move(*message),
            .respond = [=](const auto &message) { websocket->send(message); },
        };

        requests.push(std::move(request));
    }
}
}

namespace brayns
{
WebSocketHandler::WebSocketHandler(Logger &logger):
    _logger(&logger)
{
}

Request WebSocketHandler::wait()
{
    return _requests.wait();
}

void WebSocketHandler::handle(WebSocket websocket)
{
    auto client = _ids.next();

    auto ptr = std::make_shared<WebSocketWrapper>(*_logger, client, std::move(websocket));

    try
    {
        _logger->info("New client connected with ID {}", client);
        runClientLoop(_requests, ptr);
    }
    catch (const WebSocketClosed &e)
    {
        _logger->warn("WebSocket closed by peer: {}", e.what());
    }
    catch (const WebSocketException &e)
    {
        _logger->warn("Error while processing websocket: '{}'", e.what());
        ptr->close(e.getStatus(), e.what());
    }
    catch (...)
    {
        _logger->error("Unknown error in websocket handler");
        ptr->close(WebSocketStatus::UnexpectedCondition, "Internal error");
    }

    _ids.recycle(client);

    _logger->info("Client {} disconnected", client);
}
}
