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

#include "RequestHandler.h"

#include <fmt/format.h>

#include <brayns/core/json/JsonValidator.h>
#include <brayns/core/jsonrpc/Parser.h>

namespace
{
using namespace brayns::experimental;

JsonRpcRequest parseRequest(const RawRequest &request)
{
    if (request.binary)
    {
        return parseBinaryJsonRpcRequest(request.data);
    }

    return parseJsonRpcRequest(request.data);
}

const Endpoint &findEndpoint(const JsonRpcRequest &request, const EndpointRegistry &endpoints)
{
    const auto *endpoint = endpoints.find(request.method);

    if (endpoint == nullptr)
    {
        throw MethodNotFound(request.method);
    }

    return *endpoint;
}

void validateParams(const JsonRpcRequest &request, const Endpoint &endpoint)
{
    auto errors = validate(request.params, endpoint.schema.params);

    if (!errors.empty())
    {
        throw InvalidParams("Invalid params schema", errors);
    }
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

RawResult getResult(RawTask task, const Endpoint &endpoint, TaskManager &tasks, Logger &logger)
{
    if (!endpoint.schema.async)
    {
        logger.info("Result already available as request is not async");
        return task.wait();
    }

    auto id = tasks.add(std::move(task));
    logger.info("Task created with ID {}", id);

    return {serializeToJson(TaskResult{id})};
}

RawResponse executeRequest(JsonRpcRequest request, const EndpointRegistry &endpoints, TaskManager &tasks, Logger &logger)
{
    logger.info("Searching endpoint '{}'", request.method);
    const auto &endpoint = findEndpoint(request, endpoints);
    logger.info("Endpoint found");

    logger.info("Validating params schema");
    validateParams(request, endpoint);
    logger.info("Params schema valid");

    auto params = RawParams{std::move(request.params), std::move(request.binary)};

    logger.info("Calling endpoint for request {}", request.id);
    auto task = endpoint.startTask(std::move(params));
    logger.info("Successfully called endpoint");

    auto result = getResult(std::move(task), endpoint, tasks, logger);

    logger.info("Composing response");
    auto response = composeResponse(request.id, std::move(result));
    logger.info("Successfully composed response");

    return response;
}

void tryExecuteRequest(
    JsonRpcRequest request,
    const ResponseHandler &respond,
    const EndpointRegistry &endpoints,
    TaskManager &tasks,
    Logger &logger)
{
    try
    {
        auto response = executeRequest(std::move(request), endpoints, tasks, logger);
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
}

namespace brayns::experimental
{
RequestHandler::RequestHandler(const EndpointRegistry &endpoints, TaskManager &tasks, Logger &logger):
    _endpoints(&endpoints),
    _tasks(&tasks),
    _logger(&logger)
{
}

void RequestHandler::handle(const RawRequest &request)
{
    auto jsonRpcRequest = tryParseRequest(request, *_logger);

    if (!jsonRpcRequest)
    {
        return;
    }

    tryExecuteRequest(std::move(*jsonRpcRequest), request.respond, *_endpoints, *_tasks, *_logger);
}
}
