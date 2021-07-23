/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#pragma once

#include <cassert>
#include <memory>
#include <type_traits>

#include <brayns/network/messages/SchemaMessage.h>

#include "IEntrypoint.h"

namespace brayns
{
class EntrypointSchema
{
public:
    static SchemaResult create(const IEntrypoint& entrypoint)
    {
        SchemaResult schema;
        schema.title = entrypoint.getName();
        schema.type = "method";
        schema.description = entrypoint.getDescription();
        schema.async = entrypoint.isAsync();
        auto params = entrypoint.getParamsSchema();
        if (!JsonSchemaHelper::isEmpty(params))
        {
            schema.params.push_back(std::move(params));
        }
        schema.returns = entrypoint.getResultSchema();
        return schema;
    }
};

class EntrypointRef
{
public:
    template <typename T, typename... Args>
    static EntrypointRef create(Args&&... args)
    {
        static_assert(std::is_base_of<IEntrypoint, T>());
        return EntrypointRef(std::make_unique<T>(std::forward<Args>(args)...));
    }

    EntrypointRef(std::unique_ptr<IEntrypoint> entrypoint)
        : _entrypoint(std::move(entrypoint))
    {
        assert(_entrypoint);
    }

    void setup(NetworkContext& context)
    {
        _entrypoint->setContext(context);
        _entrypoint->onCreate();
        _schema = EntrypointSchema::create(*_entrypoint);
    }

    void update() const { _entrypoint->onUpdate(); }

    void processRequest(const NetworkRequest& request) const
    {
        _entrypoint->onRequest(request);
    }

    const SchemaResult& getSchema() const { return _schema; }

    const std::string& getName() const { return _schema.title; }

    const std::string& getDescription() const { return _schema.description; }

    const auto& getParamsSchema() const { return _schema.params; }

    const JsonSchema& getResultSchema() const { return _schema.returns; }

    bool isAsync() const { return _schema.async; }

private:
    std::unique_ptr<IEntrypoint> _entrypoint;
    SchemaResult _schema;
};
} // namespace brayns