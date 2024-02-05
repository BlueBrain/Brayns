/* Copyright 2015-2024 Blue Brain Project/EPFL
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
#include <brayns/network/context/NetworkContext.h>
#include <brayns/network/entrypoint/EntrypointException.h>

namespace
{
using namespace brayns;

class MessageValidator
{
public:
    static void validate(const JsonValue& params, const SchemaResult& schema)
    {
        auto& schemaParams = schema.params;
        if (schemaParams.empty())
        {
            return;
        }
        auto& paramsSchema = schemaParams[0];
        if (JsonSchemaHelper::isEmpty(paramsSchema))
        {
            return;
        }
        auto errors = JsonSchemaValidator::validate(params, paramsSchema);
        if (!errors.empty())
        {
            throw EntrypointException(0, "Invalid params", errors);
        }
    }
};

class MessageDispatcher
{
public:
    MessageDispatcher(const EntrypointManager& entrypoints)
        : _entrypoints(&entrypoints)
    {
    }

    void dispatch(const NetworkRequest& request)
    {
        auto& message = request.getMessage();
        auto& entrypoint = _getEntrypoint(message);
        _validateSchema(message, entrypoint);
        entrypoint.processRequest(request);
    }

private:
    const EntrypointRef& _getEntrypoint(const RequestMessage& message)
    {
        auto& method = message.method;
        auto entrypoint = _entrypoints->find(method);
        if (!entrypoint)
        {
            throw EntrypointException("Invalid entrypoint: '" + method + "'");
        }
        return *entrypoint;
    }

    void _validateSchema(const RequestMessage& message,
                         const EntrypointRef& entrypoint)
    {
        auto& params = message.params;
        auto& schema = entrypoint.getSchema();
        MessageValidator::validate(params, schema);
    }

    const EntrypointManager* _entrypoints;
};
} // namespace

namespace brayns
{
EntrypointManager::EntrypointManager(NetworkContext& context)
    : _context(&context)
{
}

const EntrypointRef* EntrypointManager::find(const std::string& name) const
{
    auto i = _entrypoints.find(name);
    return i == _entrypoints.end() ? nullptr : &i->second;
}

void EntrypointManager::add(EntrypointRef entrypoint)
{
    auto name = entrypoint.loadName();
    if (name.empty())
    {
        throw EntrypointException("Entrypoints must have a name");
    }
    if (find(name))
    {
        throw EntrypointException("Entrypoint '" + name + "' already exists");
    }
    Log::info("Add entrypoint '{}'.", name);
    _entrypoints.emplace(name, std::move(entrypoint));
}

void EntrypointManager::setup()
{
    for (auto& pair : _entrypoints)
    {
        auto& entrypoint = pair.second;
        entrypoint.setup(*_context);
    }
}

void EntrypointManager::update() const
{
    for (const auto& pair : _entrypoints)
    {
        auto& entrypoint = pair.second;
        entrypoint.update();
    }
}

void EntrypointManager::processRequest(const NetworkRequest& request) const
{
    MessageDispatcher dispatcher(*this);
    dispatcher.dispatch(request);
}

void EntrypointManager::preRender() const
{
    for (const auto& pair : _entrypoints)
    {
        auto& entrypoint = pair.second;
        entrypoint.preRender();
    }
}

void EntrypointManager::postRender() const
{
    for (const auto& pair : _entrypoints)
    {
        auto& entrypoint = pair.second;
        entrypoint.postRender();
    }
}
} // namespace brayns
