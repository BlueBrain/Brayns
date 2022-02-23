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

#include <brayns/network/entrypoint/EntrypointFinder.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>
#include <brayns/network/jsonrpc/JsonRpcParser.h>

namespace
{
class CurrentEntrypointWatcher
{
public:
    CurrentEntrypointWatcher(
        brayns::CurrentEntrypoint &current,
        const brayns::JsonRpcRequest &request,
        const brayns::EntrypointRef &target)
        : _current(current)
    {
        if (!_current)
        {
            _current = target;
            return;
        }
        if (request.getMethod() == "cancel")
        {
            return;
        }
        throw brayns::ServiceUnavailableException(_current.getMethod());
    }

    ~CurrentEntrypointWatcher()
    {
        _current = {};
    }

private:
    brayns::CurrentEntrypoint &_current;
};

class BinaryDispatcher
{
public:
    static void dispatch(
        brayns::CurrentEntrypoint &current,
        const brayns::ClientRequest &request,
        const brayns::EntrypointRegistry &entrypoints)
    {
        if (current)
        {
            throw brayns::ServiceUnavailableException(current.getMethod());
        }
        entrypoints.forEach([&](auto &entrypoint) { entrypoint.onBinary(request); });
    }
};

class TextDispatcher
{
public:
    static void dispatch(
        brayns::CurrentEntrypoint &current,
        const brayns::ClientRequest &request,
        const brayns::EntrypointRegistry &entrypoints)
    {
        auto jsonrpc = brayns::JsonRpcParser::parse(request);
        brayns::Log::debug("Request successfuly parsed: '{}'.", jsonrpc.getMethod());
        auto &target = brayns::EntrypointFinder::find(jsonrpc, entrypoints);
        brayns::Log::debug("Method '{}' is supported.", target.getMethod());
        _dispatch(current, jsonrpc, target);
    }

private:
    static void _dispatch(
        brayns::CurrentEntrypoint &current,
        const brayns::JsonRpcRequest &request,
        const brayns::EntrypointRef &target)
    {
        try
        {
            CurrentEntrypointWatcher watcher(current, request, target);
            brayns::Log::debug("Processing JSON-RPC request.");
            target.onRequest(request);
            brayns::Log::debug("JSON-RPC request processing done.");
        }
        catch (const brayns::JsonRpcException &e)
        {
            brayns::Log::debug("JSON-RPC error: '{}'.", e.what());
            request.error(e);
        }
        catch (const std::exception &e)
        {
            brayns::Log::debug("Unexpected JSON-RPC error: '{}'.", e.what());
            request.error({e.what()});
        }
        catch (...)
        {
            brayns::Log::debug("Unknown JSON-RPC error.");
            request.error({"Unknow error"});
        }
    }
};

class RequestDispatcherHelper
{
public:
    static void dispatch(
        brayns::CurrentEntrypoint &entrypoint,
        const brayns::ClientRequest &request,
        const brayns::EntrypointRegistry &entrypoints)
    {
        try
        {
            brayns::Log::debug("Dispatching request (current method: '{}').", entrypoint.getMethod());
            _dispatch(entrypoint, request, entrypoints);
            brayns::Log::debug("Request successfuly dispatched.");
        }
        catch (const brayns::JsonRpcException &e)
        {
            brayns::Log::debug("Request error: '{}'", e.what());
            request.error(e);
        }
        catch (const std::exception &e)
        {
            brayns::Log::error("Unexpected request error: '{}'", e.what());
            request.error({e.what()});
        }
        catch (...)
        {
            brayns::Log::error("Unknown request error.");
            request.error({"Unknown error"});
        }
    }

private:
    static void _dispatch(
        brayns::CurrentEntrypoint &entrypoint,
        const brayns::ClientRequest &request,
        const brayns::EntrypointRegistry &entrypoints)
    {
        if (request.isBinary())
        {
            brayns::Log::debug("Processing binary request");
            BinaryDispatcher::dispatch(entrypoint, request, entrypoints);
            return;
        }
        if (request.isText())
        {
            brayns::Log::debug("Processing text request");
            TextDispatcher::dispatch(entrypoint, request, entrypoints);
            return;
        }
        brayns::Log::error("Invalid packet received, neither binary not text.");
    }
};
} // namespace

namespace brayns
{
RequestDispatcher::RequestDispatcher(CurrentEntrypoint &entrypoint)
    : _entrypoint(entrypoint)
{
}

void RequestDispatcher::dispatch(const ClientRequest &request, const EntrypointRegistry &entrypoints)
{
    RequestDispatcherHelper::dispatch(_entrypoint, request, entrypoints);
}
} // namespace brayns
