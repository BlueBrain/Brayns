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

#include <brayns/common/Log.h>

#include <cassert>

#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace
{
class RequestHandler
{
public:
    static void handle(const brayns::JsonRpcRequest &request, const brayns::EntrypointRef &entrypoint, bool cancelled)
    {
        try
        {
            brayns::Log::info("Execution of JSON-RPC request {}.", request);
            _handle(request, entrypoint, cancelled);
            brayns::Log::info("Successfully executed JSON-RPC request {}.", request);
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
    static void _handle(const brayns::JsonRpcRequest &request, const brayns::EntrypointRef &entrypoint, bool cancelled)
    {
        if (cancelled)
        {
            throw brayns::TaskCancelledException();
        }
        entrypoint.onRequest(request);
    }
};

class CancelHandler
{
public:
    static void handle(const brayns::JsonRpcRequest &request, const brayns::EntrypointRef &entrypoint, bool running)
    {
        brayns::Log::info("Cancel JSON-RPC request {}.", request);
        if (!entrypoint.isAsync())
        {
            auto &method = request.getMethod();
            brayns::Log::info("Entrypoint '{}' does not support cancellation.", method);
            throw brayns::TaskNotCancellableException(method);
        }
        if (!running)
        {
            brayns::Log::debug("Anticipated cancellation of JSON-RPC request {}.", request);
            return;
        }
        brayns::Log::debug("Runtime cancellation of JSON-RPC request {}.", request);
        entrypoint.onCancel();
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
    _running = true;
    RequestHandler::handle(_request, _entrypoint, _cancelled);
    _running = false;
}

void JsonRpcTask::cancel()
{
    CancelHandler::handle(_request, _entrypoint, _running);
    _cancelled = true;
}
} // namespace brayns
