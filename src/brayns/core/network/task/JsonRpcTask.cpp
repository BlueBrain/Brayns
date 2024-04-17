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

#include "JsonRpcTask.h"

#include <cassert>

#include <fmt/format.h>

#include <brayns/core/utils/Log.h>

#include <brayns/core/network/jsonrpc/JsonRpcException.h>

namespace
{
class RequestHandler
{
public:
    static void handle(const brayns::JsonRpcRequest &request, brayns::EntrypointRef &entrypoint)
    {
        try
        {
            entrypoint.onRequest(request);
            brayns::Log::info("Successfully executed JSON-RPC request {}.", request);
        }
        catch (const brayns::JsonRpcException &e)
        {
            brayns::Log::info("Failed to execute JSON-RPC request {}: {}.", request, e);
            request.error(e);
        }
        catch (const brayns::JsonSchemaException &e)
        {
            brayns::Log::info("JSON exception during execution of JSON-RPC request {}: '{}'", request, e.what());
            request.error(brayns::InvalidParamsException(e.what(), e.getErrors()));
        }
        catch (const std::exception &e)
        {
            brayns::Log::info("Custom exception during execution of JSON-RPC request {}: '{}'.", request, e.what());
            request.error(brayns::InternalErrorException(e.what()));
        }
        catch (...)
        {
            brayns::Log::error("Unknown failure during execution of JSON-RPC request {}.", request);
            request.error(brayns::InternalErrorException("Unknown error"));
        }
    }
};
} // namespace

namespace brayns
{
JsonRpcTask::JsonRpcTask(JsonRpcRequest request, EntrypointRef &entrypoint):
    _request(std::move(request)),
    _entrypoint(entrypoint)
{
    assert(_request.getMethod() == _entrypoint.getMethod());
}

const ClientRef &JsonRpcTask::getClient() const
{
    return _request.getClient();
}

const RequestId &JsonRpcTask::getId() const
{
    return _request.getId();
}

const std::string &JsonRpcTask::getMethod() const
{
    return _request.getMethod();
}

bool JsonRpcTask::hasPriority() const
{
    return _entrypoint.hasPriority();
}

void JsonRpcTask::run()
{
    assert(!_running);
    Log::info("Execution of JSON-RPC request {}.", _request);
    if (_disconnected)
    {
        Log::info("Skipping execution of JSON-RPC request {} as client is disconnected.", _request);
        return;
    }
    if (_cancelled)
    {
        Log::info("JSON-RPC request {} has been cancelled before execution.", _request);
        _request.error(TaskCancelledException());
        return;
    }
    _running = true;
    RequestHandler::handle(_request, _entrypoint);
    _running = false;
}

void JsonRpcTask::cancel()
{
    Log::info("Cancelling JSON-RPC request {}.", _request);
    if (!_entrypoint.isAsync())
    {
        throw InvalidParamsException(fmt::format("Method '{}' is not cancellable.", _entrypoint.getMethod()));
    }
    if (_cancelled)
    {
        throw InvalidParamsException(fmt::format("Request {} already cancelled", _request.getId()));
    }
    _cancelled = true;
    if (!_running)
    {
        Log::info("Cancelling JSON-RPC request {} before execution.", _request);
        return;
    }
    Log::debug("Runtime cancellation of JSON-RPC request {}.", _request);
    _entrypoint.onCancel();
}

void JsonRpcTask::disconnect()
{
    assert(!_disconnected);
    try
    {
        Log::info("Notifying disconnection to request {}.", _request);
        _entrypoint.onDisconnect();
    }
    catch (...)
    {
        Log::error("Unexpected error during disconnection.");
    }
    _disconnected = true;
}
} // namespace brayns
