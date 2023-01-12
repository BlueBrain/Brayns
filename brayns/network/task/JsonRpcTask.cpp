/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/utils/Log.h>

#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace
{
class RequestHandler
{
public:
    RequestHandler(bool cancelled, bool disconnected)
        : _cancelled(cancelled)
        , _disconnected(disconnected)
    {
    }

    void handle(const brayns::JsonRpcRequest &request, const brayns::EntrypointRef &entrypoint)
    {
        try
        {
            _handle(request, entrypoint);
        }
        catch (const brayns::JsonRpcException &e)
        {
            brayns::Log::info("Failed to execute JSON-RPC request {}: {}.", request, e);
            request.error(e);
        }
        catch (const std::exception &e)
        {
            brayns::Log::error("Unexpected failure during execution of JSON-RPC request {}: '{}'.", request, e.what());
            request.error(brayns::InternalErrorException(e.what()));
        }
        catch (...)
        {
            brayns::Log::error("Unknown failure of during execution of JSON-RPC request {}.", request);
            request.error(brayns::InternalErrorException("Unknown error"));
        }
    }

private:
    bool _cancelled = false;
    bool _disconnected = false;

    void _handle(const brayns::JsonRpcRequest &request, const brayns::EntrypointRef &entrypoint)
    {
        if (_disconnected)
        {
            brayns::Log::info("Skipping execution of JSON-RPC request {} as client is disconnected.", request);
            return;
        }
        brayns::Log::info("Execution of JSON-RPC request {}.", request);
        if (_cancelled)
        {
            brayns::Log::info("JSON-RPC request {} has been cancelled before execution.", request);
            throw brayns::TaskCancelledException();
        }
        entrypoint.onRequest(request);
        brayns::Log::info("Successfully executed JSON-RPC request {}.", request);
    }
};

class CancelHandler
{
public:
    CancelHandler(bool running, bool cancelled)
        : _running(running)
        , _cancelled(cancelled)
    {
    }

    void handle(const brayns::JsonRpcRequest &request, const brayns::EntrypointRef &entrypoint)
    {
        brayns::Log::info("Cancelling JSON-RPC request {}.", request);
        if (!entrypoint.isAsync())
        {
            auto &method = entrypoint.getMethod();
            brayns::Log::info("Method '{}' is not cancellable.", method);
            throw brayns::InvalidParamsException("Method '" + method + "' is not cancellable");
        }
        if (_cancelled)
        {
            auto &id = request.getId();
            auto text = id.getDisplayText();
            brayns::Log::info("Request {} cancelled twice.", request);
            throw brayns::InvalidParamsException("Trying to cancel request '" + text + "' twice");
        }
        if (!_running)
        {
            brayns::Log::info("Cancelling JSON-RPC request {} before execution (entrypoint will not be called).");
            return;
        }
        brayns::Log::debug("Runtime cancellation of JSON-RPC request {}.", request);
        entrypoint.onCancel();
    }

private:
    bool _running = false;
    bool _cancelled = false;
};

class DisconnectionHandler
{
public:
    void handle(const brayns::JsonRpcRequest &request, const brayns::EntrypointRef &entrypoint)
    {
        try
        {
            brayns::Log::info("Cancelling request {} because of disconnection.", request);
            entrypoint.onDisconnect();
        }
        catch (...)
        {
            brayns::Log::error("Unexpected error during disconnection handling.");
        }
    }
};
} // namespace

namespace brayns
{
JsonRpcTask::JsonRpcTask(JsonRpcRequest request, const EntrypointRef &entrypoint)
    : _request(std::move(request))
    , _entrypoint(entrypoint)
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
    _running = true;
    RequestHandler handler(_cancelled, _disconnected);
    handler.handle(_request, _entrypoint);
    _running = false;
}

void JsonRpcTask::cancel()
{
    CancelHandler handler(_running, _cancelled);
    handler.handle(_request, _entrypoint);
    _cancelled = true;
}

void JsonRpcTask::disconnect()
{
    assert(!_disconnected);
    DisconnectionHandler handler;
    handler.handle(_request, _entrypoint);
    _disconnected = true;
}
} // namespace brayns
