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

#include <fmt/format.h>

#include <brayns/core/jsonrpc/Parser.h>

namespace
{
using namespace brayns;

JsonRpcRequest parseRequest(const RawRequest &request)
{
    if (request.binary)
    {
        return parseBinaryJsonRpcRequest(request.data);
    }

    return parseJsonRpcRequest(request.data);
}

RawResponse composeResponse(const JsonRpcId &id, RawResult result)
{
    if (result.binary.empty())
    {
        auto data = composeAsText({id, result.json});
        return {.data = std::move(data), .binary = false};
    }

    auto data = composeAsBinary({id, result.json, std::move(result.binary)});
    return {.data = std::move(data), .binary = true};
}

std::optional<JsonRpcRequest> tryParseRequest(const RawRequest &request, Logger &logger)
{
    try
    {
        logger.info("Parsing JSON-RPC request from client {}", request.clientId);
        auto jsonRpcRequest = parseRequest(request);
        logger.info("Successfully parsed request");

        return jsonRpcRequest;
    }
    catch (const JsonRpcException &e)
    {
        logger.warn("Error while parsing request: '{}'", e.what());
        request.respond({composeError(NullJson(), e)});
    }
    catch (const std::exception &e)
    {
        logger.error("Unexpected error while parsing request: '{}'", e.what());
        request.respond({composeError(NullJson(), InternalError(e.what()))});
    }
    catch (...)
    {
        logger.error("Unknown error while parsing request");
        request.respond({composeError(NullJson(), InternalError("Unknown parsing error"))});
    }

    return std::nullopt;
}

RawResponse executeRequest(JsonRpcRequest request, Api &api, Logger &logger)
{
    auto params = RawParams{std::move(request.params), std::move(request.binary)};

    logger.info("Calling endpoint for request {}", request.id);
    auto result = api.execute(request.method, std::move(params));
    logger.info("Successfully called endpoint");

    logger.info("Composing response");
    auto response = composeResponse(request.id, std::move(result));
    logger.info("Successfully composed response");

    return response;
}

void tryExecuteRequest(JsonRpcRequest request, const ResponseHandler &respond, Api &api, Logger &logger)
{
    try
    {
        auto response = executeRequest(std::move(request), api, logger);
        respond(response);
    }
    catch (const JsonRpcException &e)
    {
        logger.warn("Error during request execution: '{}'", e.what());
        respond({composeError(request.id, e)});
    }
    catch (const std::exception &e)
    {
        logger.error("Unexpected error during request execution: '{}'", e.what());
        respond({composeError(request.id, InternalError(e.what()))});
    }
    catch (...)
    {
        logger.error("Unknown error during request execution");
        respond({composeError(request.id, InternalError("Unknown handling error"))});
    }
}

void handleRequest(const RawRequest &request, Api &api, Logger &logger)
{
    auto jsonRpcRequest = tryParseRequest(request, logger);

    if (!jsonRpcRequest)
    {
        return;
    }

    tryExecuteRequest(std::move(*jsonRpcRequest), request.respond, api, logger);
}
}

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

void runService(WebSocketServer &server, Api &api, StopToken &token, Logger &logger)
{
    while (true)
    {
        logger.info("Waiting for incoming requests");
        auto requests = server.waitForRequests();

        logger.info("Received {} requests", requests.size());
        for (const auto &request : requests)
        {
            handleRequest(request, api, logger);

            if (token.isStopped())
            {
                logger.info("Service stopped");
                return;
            }
        }
    }
}
}
