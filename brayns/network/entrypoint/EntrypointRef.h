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

#include "EntrypointSchema.h"
#include "IEntrypoint.h"

namespace brayns
{
class EntrypointRef
{
public:
    template <typename T, typename... Args>
    static EntrypointRef of(Args&&... args)
    {
        static_assert(std::is_base_of<IEntrypoint, T>());
        return EntrypointRef(std::make_unique<T>(std::forward<Args>(args)...));
    }

    EntrypointRef(std::unique_ptr<IEntrypoint> entrypoint)
        : _entrypoint(std::move(entrypoint))
    {
        assert(_entrypoint);
        _schema = EntrypointSchemaFactory::createSchema(*_entrypoint);
    }

    void setup(PluginAPI& api, ClientRegistry& clients)
    {
        _entrypoint->setApi(api);
        _entrypoint->setClientRegistry(clients);
        _entrypoint->onCreate();
    }

    void processRequest(const NetworkRequest& request) const
    {
        _entrypoint->onRequest(request);
    }

    const EntrypointSchema& getSchema() const { return _schema; }

    const std::string& getName() const { return _schema.title; }

    const std::string& getDescription() const { return _schema.description; }

    const auto& getParamsSchema() const { return _schema.params; }

    const JsonSchema& getResultSchema() const { return _schema.returns; }

private:
    std::unique_ptr<IEntrypoint> _entrypoint;
    EntrypointSchema _schema;
};
} // namespace brayns