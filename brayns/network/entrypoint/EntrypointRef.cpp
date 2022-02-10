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

namespace brayns
{
EntrypointRef::EntrypointRef(std::string plugin, std::unique_ptr<IEntrypoint> entrypoint)
    : _entrypoint(std::move(entrypoint))
{
    assert(_entrypoint);
    _schema.plugin = std::move(plugin);
    _schema.title = _entrypoint->getName();
}

void EntrypointRef::onCreate()
{
    _entrypoint->onCreate();
    _schema.type = "method";
    _schema.description = _entrypoint->getDescription();
    _schema.async = _entrypoint->isAsync();
    auto params = _entrypoint->getParamsSchema();
    if (!brayns::JsonSchemaHelper::isEmpty(params))
    {
        _schema.params.push_back(std::move(params));
    }
    _schema.returns = _entrypoint->getResultSchema();
}

void EntrypointRef::onUpdate() const
{
    _entrypoint->onUpdate();
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

const std::string &EntrypointRef::getPlugin() const
{
    return _schema.plugin;
}

const SchemaResult &EntrypointRef::getSchema() const
{
    return _schema;
}

const std::string &EntrypointRef::getName() const
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
} // namespace brayns
