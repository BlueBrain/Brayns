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

#include "EntrypointManager.h"

#include <brayns/common/Log.h>

#include <brayns/json/JsonSchemaValidator.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace
{
class MessageValidator
{
public:
    static void validate(const brayns::JsonValue &params, const brayns::SchemaResult &schema)
    {
        if (schema.params.empty())
        {
            _checkParamsIsEmpty(params);
            return;
        }
        _validate(params, schema.params[0]);
    }

private:
    static void _checkParamsIsEmpty(const brayns::JsonValue &params)
    {
        if (!params.isEmpty())
        {
            throw brayns::InvalidParamsException("This method takes no params");
        }
    }

    static void _validate(const brayns::JsonValue &params, const brayns::JsonSchema &schema)
    {
        auto errors = brayns::JsonSchemaValidator::validate(params, schema);
        if (!errors.empty())
        {
            throw brayns::InvalidParamsException("Invalid params schema", errors);
        }
    }
};

class MessageDispatcher
{
public:
    static void dispatch(const brayns::JsonRpcRequest &request, const brayns::EntrypointManager &entrypoints)
    {
        try
        {
            _dispatch(request, entrypoints);
        }
        catch (const brayns::JsonRpcException &e)
        {
            (void)e;
            throw;
        }
        catch (const std::exception &e)
        {
            throw brayns::InternalErrorException("Unexpected error during request dispatch: " + std::string(e.what()));
        }
        catch (...)
        {
            throw brayns::InternalErrorException("Unknown error during request dispatch");
        }
    }

private:
    static void _dispatch(const brayns::JsonRpcRequest &request, const brayns::EntrypointManager &entrypoints)
    {
        auto &message = request.getMessage();
        auto &entrypoint = _getEntrypoint(message, entrypoints);
        _validateSchema(message, entrypoint);
        entrypoint.onRequest(request);
    }

    static const brayns::EntrypointRef &_getEntrypoint(
        const brayns::RequestMessage &message,
        const brayns::EntrypointManager &entrypoints)
    {
        auto &method = message.method;
        auto entrypoint = entrypoints.find(method);
        if (!entrypoint)
        {
            throw brayns::MethodNotFoundException(method);
        }
        return *entrypoint;
    }

    static void _validateSchema(const brayns::RequestMessage &message, const brayns::EntrypointRef &entrypoint)
    {
        auto &params = message.params;
        auto &schema = entrypoint.getSchema();
        MessageValidator::validate(params, schema);
    }
};
} // namespace

namespace brayns
{
const EntrypointRef *EntrypointManager::find(const std::string &name) const
{
    auto i = _entrypoints.find(name);
    return i == _entrypoints.end() ? nullptr : &i->second;
}

std::vector<std::string> EntrypointManager::getNames() const
{
    std::vector<std::string> names;
    names.reserve(_entrypoints.size());
    for (const auto &[name, entrypoint] : _entrypoints)
    {
        names.push_back(name);
    }
    return names;
}

void EntrypointManager::add(EntrypointRef entrypoint)
{
    auto &name = entrypoint.getName();
    if (name.empty())
    {
        throw std::invalid_argument("Entrypoints must have a name");
    }
    if (find(name))
    {
        throw std::invalid_argument("Entrypoint '" + name + "' already registered");
    }
    Log::info("Register entrypoint '{}'.", name);
    _entrypoints.emplace(name, std::move(entrypoint));
}

void EntrypointManager::onCreate()
{
    for (auto &[name, entrypoint] : _entrypoints)
    {
        entrypoint.onCreate();
    }
}

void EntrypointManager::onUpdate() const
{
    for (const auto &[name, entrypoint] : _entrypoints)
    {
        entrypoint.onUpdate();
    }
}

void EntrypointManager::onRequest(const JsonRpcRequest &request) const
{
    MessageDispatcher::dispatch(request, *this);
}

void EntrypointManager::onPreRender() const
{
    for (const auto &[name, entrypoint] : _entrypoints)
    {
        entrypoint.onPreRender();
    }
}

void EntrypointManager::onPostRender() const
{
    for (const auto &[name, entrypoint] : _entrypoints)
    {
        entrypoint.onPostRender();
    }
}
} // namespace brayns
