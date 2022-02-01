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
class EntrypointSchema
{
public:
    static brayns::SchemaResult create(const brayns::IEntrypoint &entrypoint)
    {
        brayns::SchemaResult schema;
        schema.plugin = entrypoint.getPlugin();
        schema.title = entrypoint.getName();
        schema.type = "method";
        schema.description = entrypoint.getDescription();
        schema.async = entrypoint.isAsync();
        auto params = entrypoint.getParamsSchema();
        if (!brayns::JsonSchemaHelper::isEmpty(params))
        {
            schema.params.push_back(std::move(params));
        }
        schema.returns = entrypoint.getResultSchema();
        return schema;
    }
};
} // namespace

namespace brayns
{
EntrypointRef::EntrypointRef(std::unique_ptr<IEntrypoint> entrypoint)
    : _entrypoint(std::move(entrypoint))
{
    assert(_entrypoint);
}

void EntrypointRef::setup(NetworkContext &context)
{
    _entrypoint->setContext(context);
    _entrypoint->onCreate();
    _schema = EntrypointSchema::create(*_entrypoint);
}

void EntrypointRef::update() const
{
    _entrypoint->onUpdate();
}

void EntrypointRef::processRequest(const NetworkRequest &request) const
{
    _entrypoint->onRequest(request);
}

void EntrypointRef::preRender() const
{
    _entrypoint->onPreRender();
}

void EntrypointRef::postRender() const
{
    _entrypoint->onPostRender();
}

std::string EntrypointRef::loadName() const
{
    return _entrypoint->getName();
}

const std::string &EntrypointRef::getPlugin() const
{
    return _entrypoint->getPlugin();
}

void EntrypointRef::setPlugin(const std::string &plugin)
{
    _entrypoint->setPlugin(plugin);
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

bool EntrypointRef::isAsync() const
{
    return _schema.async;
}
} // namespace brayns
