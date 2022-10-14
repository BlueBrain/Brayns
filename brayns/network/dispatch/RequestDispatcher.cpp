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

#include <brayns/utils/Log.h>

#include <brayns/network/entrypoint/EntrypointFinder.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>
#include <brayns/network/jsonrpc/JsonRpcParser.h>
#include <brayns/network/task/JsonRpcTask.h>

namespace
{
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
            brayns::Log::debug("Dispatch JSON-RPC request {} to entrypoints.", request);
            auto &entrypoint = brayns::EntrypointFinder::find(request, entrypoints);
            brayns::Log::debug("Entrypoint found to process request: '{}'.", entrypoint.getMethod());
            auto task = std::make_unique<brayns::JsonRpcTask>(std::move(request), entrypoint);
            tasks.add(std::move(task));
        }
        catch (const brayns::JsonRpcException &e)
        {
            brayns::Log::info("Failed to dispatch request {} to entrypoints: {}.", request, e);
            request.error(e);
        }
        catch (const std::exception &e)
        {
            brayns::Log::error("Unexpected error in dispatch of request {} to entrypoints: '{}'.", request, e.what());
            request.error({e.what()});
        }
        catch (...)
        {
            brayns::Log::error("Unknown error in dispatch of request {} to entrypoints.", request);
            request.error({"Unknown error"});
        }
    }
};

class RawRequestDispatcher
{
public:
    static void dispatch(
        const brayns::ClientRequest &request,
        const brayns::EntrypointRegistry &entrypoints,
        brayns::TaskManager &tasks)
    {
        try
        {
            brayns::Log::debug("Parse JSON-RPC request.");
            auto jsonrpc = brayns::JsonRpcParser::parse(request);
            brayns::Log::info("Successfully parsed JSON-RPC request {}.", jsonrpc);
            JsonRpcDispatcher::dispatch(std::move(jsonrpc), entrypoints, tasks);
        }
        catch (const brayns::JsonRpcException &e)
        {
            brayns::Log::info("Failed to parse request {}: {}.", request, e);
            request.error(e);
        }
        catch (const std::exception &e)
        {
            brayns::Log::error("Unexpected error in parsing of request {}: '{}'.", request, e.what());
            request.error({e.what()});
        }
        catch (...)
        {
            brayns::Log::error("Unknown error in parsing of request {}.", request);
            request.error({"Unknown error"});
        }
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
    Log::debug("Dispatch request {}.", request);
    RawRequestDispatcher::dispatch(request, _entrypoints, _tasks);
    Log::debug("Request dispatched {}.", request);
}
} // namespace brayns
