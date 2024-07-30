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

struct ResponseData
{
    std::string data;
    bool binary = false;
};

JsonRpcRequest parseRequest(const RawRequest &request)
{
    if (request.binary)
    {
        return parseBinaryJsonRpcRequest(request.data);
    }

    return parseTextJsonRpcRequest(request.data);
}

ResponseData composeResponse(const JsonRpcId &id, RawResult result)
{
    if (result.binary.empty())
    {
        auto data = composeAsText({id, result.json});
        return {.data = std::move(data), .binary = false};
    }

    auto data = composeAsBinary({id, result.json, std::move(result.binary)});
    return {.data = std::move(data), .binary = true};
}

void sendExecutionError(
    const std::optional<JsonRpcId> &id,
    const JsonRpcException &e,
    const ResponseHandler &respond,
    Logger &logger)
{
    if (!id)
    {
        logger.info("No ID in request, skipping error response");
        return;
    }

    logger.info("Composing execution error");
    auto text = composeError(*id, e);
    logger.info("Execution error composed");

    logger.info("Sending execution error");
    respond({std::move(text)});
    logger.info("Execution error sent");
}

void sendParsingError(const JsonRpcException &e, const ResponseHandler &respond, Logger &logger)
{
    logger.info("Composing parsing error");
    auto text = composeError(std::nullopt, e);
    logger.info("Parsing error composed");

    logger.info("Sending parsing error");
    respond({std::move(text)});
    logger.info("Parsing error sent");
}

void executeRequest(JsonRpcRequest request, const ResponseHandler &respond, Api &api, Logger &logger)
{
    try
    {
        auto params = RawParams{std::move(request.params), std::move(request.binary)};

        logger.info("Calling endpoint for request {}", toString(request.id));
        auto result = api.execute(request.method, std::move(params));
        logger.info("Successfully called endpoint");

        if (!request.id)
        {
            logger.info("No ID in request, skipping response");
            return;
        }

        logger.info("Composing response");
        auto response = composeResponse(*request.id, std::move(result));
        logger.info("Successfully composed response");

        logger.info("Sending response");
        respond({response.data, response.binary});
        logger.info("Response sent");
    }
    catch (const JsonRpcException &e)
    {
        logger.warn("Error during request execution: '{}'", e.what());
        sendExecutionError(request.id, e, respond, logger);
    }
    catch (const std::exception &e)
    {
        logger.error("Unexpected error during request execution: '{}'", e.what());
        sendExecutionError(request.id, InternalError(e.what()), respond, logger);
    }
    catch (...)
    {
        logger.error("Unknown error during request execution");
        sendExecutionError(request.id, InternalError("Unknown execution error"), respond, logger);
    }
}

void handleRequest(const RawRequest &request, Api &api, Logger &logger)
{
    try
    {
        logger.info("Parsing JSON-RPC request from client {}", request.clientId);
        auto jsonRpcRequest = parseRequest(request);
        logger.info("Successfully parsed request");

        executeRequest(std::move(jsonRpcRequest), request.respond, api, logger);
    }
    catch (const JsonRpcException &e)
    {
        logger.warn("Error while parsing request: '{}'", e.what());
        sendParsingError(e, request.respond, logger);
    }
    catch (const std::exception &e)
    {
        logger.error("Unexpected error while parsing request: '{}'", e.what());
        sendParsingError(InternalError(e.what()), request.respond, logger);
    }
    catch (...)
    {
        logger.error("Unknown error while parsing request");
        sendParsingError(InternalError("Unknown parsing error"), request.respond, logger);
    }
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
