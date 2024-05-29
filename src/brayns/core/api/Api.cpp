/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "Api.h"

#include <stdexcept>

#include <fmt/format.h>

#include <brayns/core/jsonrpc/Errors.h>

namespace brayns::experimental
{
Api::Api(std::map<std::string, Endpoint> endpoints):
    _endpoints(std::move(endpoints))
{
}

std::vector<std::string> Api::getMethods() const
{
    auto result = std::vector<std::string>();
    result.reserve(_endpoints.size());

    for (const auto &[method, endpoint] : _endpoints)
    {
        result.push_back(method);
    }

    return result;
}

EndpointSchema Api::getSchema(const std::string &method) const
{
    auto i = _endpoints.find(method);

    if (i == _endpoints.end())
    {
        throw InvalidParams(fmt::format("Params method not found: '{}'", method));
    }

    return i->second.schema;
}

JsonRpcResponse Api::execute(const JsonRpcRequest &request)
{
    auto i = _endpoints.find(request.method);

    if (i == _endpoints.end())
    {
        throw MethodNotFound(request.method);
    }

    const auto &endpoint = i->second;

    if (!endpoint.schema.binary_params && !request.binary.empty())
    {
        throw InvalidParams("Binary params not supported for this endpoint");
    }

    auto errors = validate(request.params, endpoint.schema.params);

    if (!errors.empty())
    {
        throw InvalidParams("Invalid params", errors);
    }

    auto result = endpoint.run({request.params, request.binary});

    if (!endpoint.schema.binary_result && !result.binary.empty())
    {
        throw InternalError("Endpoint returned unsupported binary result");
    }

    return JsonRpcResponse{
        .id = request.id,
        .result = result.json,
        .binary = std::move(result.binary),
    };
}

Api ApiBuilder::build()
{
    auto endpoints = std::map<std::string, Endpoint>();

    for (auto &endpoint : _endpoints)
    {
        auto i = endpoints.find(endpoint.schema.method);

        if (i != endpoints.end())
        {
            throw std::invalid_argument("Duplicated endpoint method");
        }

        endpoints[endpoint.schema.method] = std::move(endpoint);
    }

    _endpoints.clear();

    return Api(std::move(endpoints));
}
}
