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

#include "Api.h"

#include <fmt/format.h>

#include <brayns/core/jsonrpc/Errors.h>
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

Message composeSuccessResponse(const JsonRpcSuccessResponse &response)
{
    if (response.result.binary.empty())
    {
        return {.data = composeAsText(response), .binary = false};
    }

    return {.data = composeAsBinary(response), .binary = true};
}

void sendSuccessResponse(Payload result, const ResponseHandler &handler, JsonRpcId id)
{
    auto response = JsonRpcSuccessResponse{std::move(id), std::move(result)};
    auto message = composeSuccessResponse(response);
    handler(message);
}

void sendSuccessResponseIfNeeded(Payload result, const ResponseHandler &handler, std::optional<JsonRpcId> id)
{
    if (id)
    {
        sendSuccessResponse(std::move(result), handler, std::move(*id));
    }
}

Message composeErrorResponse(const JsonRpcErrorResponse &response)
{
    return {.data = composeAsText(response), .binary = false};
}

void sendErrorResponse(const JsonRpcException &e, const ResponseHandler &handler, std::optional<JsonRpcId> id = {})
{
    auto response = JsonRpcErrorResponse{composeError(e), std::move(*id)};
    auto message = composeErrorResponse(response);
    handler(message);
}

void sendErrorResponseIfNeeded(const JsonRpcException &e, const ResponseHandler &handler, std::optional<JsonRpcId> id)
{
    if (id)
    {
        sendErrorResponse(e, handler, std::move(id));
    }
}

class ApiTask
{
public:
    explicit ApiTask(Logger &logger, std::optional<JsonRpcId> id, EndpointTask task, ResponseHandler handler):
        _logger(&logger),
        _id(std::move(id)),
        _task(std::move(task)),
        _handler(std::move(handler))
    {
    }

    void run()
    {
        try
        {
            _logger->info("Executing request {}", toString(_id));
            auto result = _task.run();
            _logger->info("Executed request");

            sendSuccessResponseIfNeeded(std::move(result), _handler, _id);
        }
        catch (const JsonRpcException &e)
        {
            _logger->warn("Execution error: {}", e.what());
            sendErrorResponseIfNeeded(e, _handler, _id);
        }
        catch (const std::exception &e)
        {
            _logger->error("Internal execution error: {}", e.what());
            sendErrorResponseIfNeeded(InternalError(e), _handler, _id);
        }
        catch (...)
        {
            _logger->error("Unknown execution error");
            sendErrorResponseIfNeeded(InternalError("Unknown parse error"), _handler, _id);
        }
    }

    TaskOperation getCurrentOperation() const
    {
        return _task.getCurrentOperation();
    }

    void cancel()
    {
        _task.cancel();
    }

private:
    Logger *_logger;
    std::optional<JsonRpcId> _id;
    EndpointTask _task;
    ResponseHandler _handler;
};

ManagedTask createTask(Logger &logger, const EndpointRegistry &endpoints, JsonRpcRequest request, ResponseHandler handler)
{
    auto task = endpoints.start(request.method, std::move(request.params));
    auto priority = task.hasPriority();

    auto ptr = std::make_shared<ApiTask>(logger, request.id, std::move(task), std::move(handler));

    return {
        .id = std::move(request.id),
        .priority = priority,
        .run = [=] { ptr->run(); },
        .getCurrentOperation = [=] { return ptr->getCurrentOperation(); },
        .cancel = [=] { ptr->cancel(); },
    };
}

void executeRequest(Logger &logger, const EndpointRegistry &endpoints, TaskManager &tasks, JsonRpcRequest request, const ResponseHandler &handler)
{
    auto id = request.id;

    try
    {
        logger.info("Scheduling request for execution");
        auto task = createTask(logger, endpoints, std::move(request), handler);
        logger.info("Scheduled request for execution");

        tasks.add(std::move(task));
    }
    catch (const JsonRpcException &e)
    {
        logger.warn("Schedule error: {}", e.what());
        sendErrorResponseIfNeeded(e, handler, id);
    }
    catch (const std::exception &e)
    {
        logger.error("Internal schedule error: {}", e.what());
        sendErrorResponseIfNeeded(InternalError(e), handler, id);
    }
    catch (...)
    {
        logger.error("Unknown schedule error");
        sendErrorResponseIfNeeded(InternalError("Unknown parse error"), handler, id);
    }
}
}

namespace brayns
{
Api::Api(Logger &logger, TaskManager &tasks, const EndpointRegistry &endpoints):
    _logger(&logger),
    _tasks(&tasks),
    _endpoints(&endpoints)
{
}

void Api::execute(const Request &request)
{
    try
    {
        _logger->info("Parsing request");
        auto parsed = parseRequest(request.message);
        _logger->info("Parsed request");

        executeRequest(*_logger, *_endpoints, *_tasks, std::move(parsed), request.respond);
    }
    catch (const JsonRpcException &e)
    {
        _logger->warn("Parse error: {}", e.what());
        sendErrorResponse(e, request.respond);
    }
    catch (const std::exception &e)
    {
        _logger->error("Internal parse error: {}", e.what());
        sendErrorResponse(InternalError(e), request.respond);
    }
    catch (...)
    {
        _logger->error("Unknown parse error");
        sendErrorResponse(InternalError("Unknown parse error"), request.respond);
    }
}
}
