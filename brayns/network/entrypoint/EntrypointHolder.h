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

#include "IEntrypoint.h"
#include "EntrypointSchema.h"

namespace brayns
{
class EntrypointHolder
{
public:
    EntrypointHolder() = default;

    EntrypointHolder(EntrypointPtr entrypoint)
        : _entrypoint(std::move(entrypoint))
        , _schema(EntrypointSchemaFactory::createSchema(*_entrypoint))
    {
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
    EntrypointPtr _entrypoint;
    EntrypointSchema _schema;
};
} // namespace brayns