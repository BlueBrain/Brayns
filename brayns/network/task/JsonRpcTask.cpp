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
#include <brayns/network/jsonrpc/JsonRpcHandler.h>

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

void JsonRpcTask::run()
{
    Log::debug("Execute JSON-RPC task for request {}.", _request);
    if (_cancelled)
    {
        Log::info("Request {} cancelled before start.", _request);
        _request.reply(TaskCancelledException());
    }
    JsonRpcHandler::handle(_request, _entrypoint);
}

void JsonRpcTask::cancel()
{
    Log::info("Cancel JSON-RPC request {}.", _request);
    if (!_entrypoint.isAsync())
    {
        Log::info("Entrypoint does not support cancellation.");
        auto &method = _request.getMethod();
        throw TaskNotCancellableException(method);
    }
    _cancelled = true;
    _entrypoint.onCancel();
}
} // namespace brayns
