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

#include "EntrypointRef.h"

#include <cassert>

namespace
{
class EntrypointSchemaBuilder
{
public:
    static void build(brayns::SchemaResult &schema, brayns::IEntrypoint &entrypoint)
    {
        schema.description = entrypoint.getDescription();
        schema.async = entrypoint.isAsync();
        auto params = entrypoint.getParamsSchema();
        if (!brayns::JsonSchemaHelper::isNull(params))
        {
            schema.params.push_back(std::move(params));
        }
        schema.returns = entrypoint.getResultSchema();
    }
};
} // namespace

namespace brayns
{
EntrypointRef::EntrypointRef(std::string plugin, std::unique_ptr<IEntrypoint> entrypoint)
    : _entrypoint(std::move(entrypoint))
{
    assert(_entrypoint);
    _schema.plugin = std::move(plugin);
    _schema.title = _entrypoint->getMethod();
}

void EntrypointRef::onCreate()
{
    _entrypoint->onCreate();
    EntrypointSchemaBuilder::build(_schema, *_entrypoint);
}

void EntrypointRef::onRequest(const JsonRpcRequest &request) const
{
    _entrypoint->onRequest(request);
}

void EntrypointRef::onPreRender() const
{
    _entrypoint->onPreRender();
}

void EntrypointRef::onPostRender() const
{
    _entrypoint->onPostRender();
}

void EntrypointRef::onCancel() const
{
    _entrypoint->onCancel();
}

void EntrypointRef::onConnect(const ClientRef &client) const
{
    _entrypoint->onConnect(client);
}

void EntrypointRef::onDisconnect(const ClientRef &client) const
{
    _entrypoint->onDisconnect(client);
}

const std::string &EntrypointRef::getPlugin() const
{
    return _schema.plugin;
}

const SchemaResult &EntrypointRef::getSchema() const
{
    return _schema;
}

const std::string &EntrypointRef::getMethod() const
{
    return _schema.title;
}

const std::string &EntrypointRef::getDescription() const
{
    return _schema.description;
}

const std::vector<JsonSchema> &EntrypointRef::getParamsSchema() const
{
    return _schema.params;
}

const JsonSchema &EntrypointRef::getResultSchema() const
{
    return _schema.returns;
}

bool EntrypointRef::isAsync() const
{
    return _schema.async;
}

bool EntrypointRef::hasPriority() const
{
    return _entrypoint->hasPriority();
}
} // namespace brayns
