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

#include "RequestDispatcher.h"

#include <brayns/common/Log.h>

#include <brayns/network/common/ErrorHandler.h>
#include <brayns/network/entrypoint/EntrypointFinder.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>
#include <brayns/network/jsonrpc/JsonRpcParser.h>

namespace
{
class EntrypointDispatcher
{
public:
    static void
        dispatch(brayns::JsonRpcRequest request, const brayns::EntrypointRef &entrypoint, brayns::TaskManager &tasks)
    {
        if (entrypoint.canBeNested())
        {
            _run(request, entrypoint);
            return;
        }
        _queue(std::move(request), entrypoint, tasks);
    }

private:
    static void _run(const brayns::JsonRpcRequest &request, const brayns::EntrypointRef &entrypoint)
    {
        try
        {
            brayns::Log::debug("Processing directly request '{}'.", request.getMethod());
            entrypoint.onRequest(request);
        }
        catch (...)
        {
            brayns::Log::error("JSON-RPC request processing failed.");
            brayns::ErrorHandler::reply(request);
        }
    }

    static void
        _queue(brayns::JsonRpcRequest request, const brayns::EntrypointRef &entrypoint, brayns::TaskManager &tasks)
    {
        brayns::Log::debug("Create task to process request '{}'.", request.getMethod());
        tasks.addJsonRpcTask(std::move(request), entrypoint);
    }
};

class JsonRpcDispatcher
{
public:
    static void dispatch(
        brayns::JsonRpcRequest request,
        const brayns::EntrypointRegistry &entrypoints,
        brayns::TaskManager &tasks)
    {
        try
        {
            brayns::Log::debug("Dispatching JSON-RPC request.");
            auto &entrypoint = brayns::EntrypointFinder::find(request, entrypoints);
            brayns::Log::debug("Entrypoint found to process request: '{}'.", entrypoint.getMethod());
            EntrypointDispatcher::dispatch(std::move(request), entrypoint, tasks);
        }
        catch (...)
        {
            brayns::Log::error("JSON-RPC request dispatch failed.");
            brayns::ErrorHandler::reply(request);
        }
    }
};

class TextDispatcher
{
public:
    static void dispatch(
        const brayns::ClientRequest &request,
        const brayns::EntrypointRegistry &entrypoints,
        brayns::TaskManager &tasks)
    {
        try
        {
            brayns::Log::debug("Parsing JSON-RPC request.");
            auto jsonrpc = brayns::JsonRpcParser::parse(request);
            JsonRpcDispatcher::dispatch(std::move(jsonrpc), entrypoints, tasks);
        }
        catch (...)
        {
            brayns::Log::error("JSON-RPC request parsing failed.");
            brayns::ErrorHandler::reply(request);
        }
    }
};

class BinaryDispatcher
{
public:
    static void dispatch(
        brayns::ClientRequest request,
        const brayns::EntrypointRegistry &entrypoints,
        brayns::TaskManager &tasks)
    {
        brayns::Log::debug("Create task to process binary request.");
        tasks.addBinaryTask(std::move(request), entrypoints);
    }
};

class RequestDispatcherHelper
{
public:
    static void dispatch(
        brayns::ClientRequest request,
        const brayns::EntrypointRegistry &entrypoints,
        brayns::TaskManager &tasks)
    {
        if (request.isBinary())
        {
            BinaryDispatcher::dispatch(std::move(request), entrypoints, tasks);
            return;
        }
        if (request.isText())
        {
            TextDispatcher::dispatch(request, entrypoints, tasks);
            return;
        }
        brayns::Log::error("Invalid request, discard it.");
    }
};
} // namespace

namespace brayns
{
RequestDispatcher::RequestDispatcher(const EntrypointRegistry &entrypoints, TaskManager &tasks)
    : _entrypoints(entrypoints)
    , _tasks(tasks)
{
}

void RequestDispatcher::dispatch(ClientRequest request)
{
    RequestDispatcherHelper::dispatch(std::move(request), _entrypoints, _tasks);
}
} // namespace brayns
