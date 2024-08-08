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

JsonRpcRequest parseRequest(const Message &message)
{
    if (message.binary)
    {
        return parseBinaryJsonRpcRequest(message.data);
    }

    return parseTextJsonRpcRequest(message.data);
}

Message composeResponse(const JsonRpcSuccessResponse &response)
{
    if (response.result.binary.empty())
    {
        return {.data = composeAsText(response), .binary = false};
    }

    return {.data = composeAsBinary(response), .binary = true};
}

Message composeResponse(const JsonRpcErrorResponse &response)
{
    return {.data = composeAsText(response), .binary = false};
}

std::optional<JsonRpcResponse> execute(JsonRpcRequest request, Api &api, Logger &logger)
{
    try
    {
        logger.info("Calling endpoint for request {}", toString(request.id));
        auto result = api.execute(request.method, std::move(request.params));
        logger.info("Successfully called endpoint");

        if (!request.id)
        {
            logger.info("No ID in request, skipping response");
            return {};
        }

        return JsonRpcSuccessResponse{*request.id, std::move(result)};
    }
    catch (const JsonRpcException &e)
    {
        logger.warn("Error during request execution: '{}'", e.what());
        return composeErrorResponse(e, *request.id);
    }
    catch (const std::exception &e)
    {
        logger.error("Unexpected error during request execution: '{}'", e.what());
        return composeErrorResponse(InternalError(e), *request.id);
    }
    catch (...)
    {
        logger.error("Unknown error during request execution");
        return composeErrorResponse(InternalError("Unknown execution error"), *request.id);
    }
}

std::optional<JsonRpcResponse> parseAndExecute(const Message &message, Api &api, Logger &logger)
{
    try
    {
        logger.info("Parsing request");
        auto request = parseRequest(message);
        logger.info("Successfully parsed request");

        return execute(std::move(request), api, logger);
    }
    catch (const JsonRpcException &e)
    {
        logger.warn("Error while parsing request: '{}'", e.what());
        return composeErrorResponse(e);
    }
    catch (const std::exception &e)
    {
        logger.error("Unexpected error while parsing request: '{}'", e.what());
        return composeErrorResponse(InternalError(e));
    }
    catch (...)
    {
        logger.error("Unknown error while parsing request");
        return composeErrorResponse(InternalError("Unknown parsing error"));
    }
}

void handleRequest(const Request &request, Api &api, Logger &logger)
{
    logger.info("Handling JSON-RPC request from client {}", request.clientId);

    auto response = parseAndExecute(request.message, api, logger);

    if (!response)
    {
        return;
    }

    logger.info("Composing response");
    auto message = std::visit([](const auto &value) { return composeResponse(value); }, *response);
    logger.info("Successfully composed response");

    logger.info("Sending response");
    request.respond(message);
    logger.info("Response sent");
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

void runService(RequestProvider waitForRequests, Api &api, StopToken &token, Logger &logger)
{
    while (true)
    {
        logger.info("Waiting for incoming requests");
        auto requests = waitForRequests();
        logger.info("Received {} requests", requests.size());

        for (const auto &request : requests)
        {
            try
            {
                handleRequest(request, api, logger);
            }
            catch (...)
            {
                logger.error("Unknown error while handling request");
            }

            if (token.isStopped())
            {
                logger.info("Service stopped");
                return;
            }
        }
    }
}
}
