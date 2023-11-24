/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#include "SchemaEntrypoint.h"

#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace brayns
{
SchemaEntrypoint::SchemaEntrypoint(EntrypointRegistry &entrypoints):
    _entrypoints(entrypoints)
{
}

std::string SchemaEntrypoint::getMethod() const
{
    return "schema";
}

std::string SchemaEntrypoint::getDescription() const
{
    return "Get the JSON schema of the given entrypoint";
}

void SchemaEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto &endpoint = params.endpoint;
    auto entrypoint = _entrypoints.find(endpoint);
    if (!entrypoint)
    {
        throw MethodNotFoundException(endpoint);
    }
    auto &schema = entrypoint->getSchema();
    request.reply(schema);
}
} // namespace brayns
