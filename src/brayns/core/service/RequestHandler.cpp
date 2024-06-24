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

#include <brayns/core/jsonrpc/Parser.h>
#include <brayns/core/jsonv2/JsonValidator.h>

namespace
{
using namespace brayns::experimental;
using brayns::Logger;

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
        logger.warn("Error while preparing request: '{}'", e.what());
        request.respond({composeError(NullJson(), e)});
    }
    catch (const std::exception &e)
    {
        logger.error("Unexpected error while preparing request: '{}'", e.what());
        request.respond({composeError(NullJson(), InternalError(e.what()))});
    }
    catch (...)
    {
        logger.error("Unknown error while preparing request");
        request.respond({composeError(NullJson(), InternalError("Unknown parsing error"))});
    }

    return std::nullopt;
}

RawResponse executeRequest(JsonRpcRequest request, const EndpointRegistry &endpoints, Logger &logger)
{
    logger.info("Looking for an endpoint '{}'", request.method);
    const auto &endpoint = findEndpoint(request, endpoints);
    logger.info("Endpoint found");

    logger.info("Validating params schema");
    validateParams(request, endpoint);
    logger.info("Params schema valid");

    auto params = RawParams{std::move(request.params), std::move(request.binary)};

    logger.info("Executing request {}", request.id);
    auto result = endpoint.run(std::move(params));
    logger.info("Successfully executed request");

    logger.info("Composing response");
    auto response = composeResponse(request.id, std::move(result));
    logger.info("Successfully composed response");

    return response;
}

void tryExecuteRequest(
    JsonRpcRequest request,
    const EndpointRegistry &endpoints,
    const ResponseHandler &respond,
    Logger &logger)
{
    try
    {
        auto response = executeRequest(std::move(request), endpoints, logger);
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
RequestHandler::RequestHandler(const EndpointRegistry &endpoints, Logger &logger):
    _endpoints(&endpoints),
    _logger(&logger)
{
}

void RequestHandler::handle(RawRequest request)
{
    auto jsonRpcRequest = tryParseRequest(std::move(request), *_logger);

    if (!jsonRpcRequest)
    {
        return;
    }

    tryExecuteRequest(std::move(*jsonRpcRequest), *_endpoints, request.respond, *_logger);
}
}
