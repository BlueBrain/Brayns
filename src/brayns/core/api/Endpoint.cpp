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

#include "Endpoint.h"

#include <brayns/core/json/JsonValidator.h>

namespace brayns
{
EndpointTask::EndpointTask(const Endpoint &endpoint, Payload params):
    _endpoint(&endpoint),
    _params(std::move(params))
{
}

bool EndpointTask::hasPriority() const
{
    return _endpoint->priority;
}

Task EndpointTask::start()
{
    return _endpoint->start(std::move(_params));
}

std::vector<std::string> EndpointRegistry::getMethods() const
{
    auto result = std::vector<std::string>();
    result.reserve(_endpoints.size());

    for (const auto &[method, endpoint] : _endpoints)
    {
        result.push_back(method);
    }

    return result;
}

const EndpointSchema &EndpointRegistry::getSchema(const std::string &method) const
{
    auto i = _endpoints.find(method);

    if (i == _endpoints.end())
    {
        throw InvalidParams(fmt::format("Unknown endpoint method: '{}'", method));
    }

    return i->second.schema;
}

EndpointTask EndpointRegistry::createTask(const std::string &method, Payload params) const
{
    auto i = _endpoints.find(method);

    if (i == _endpoints.end())
    {
        throw MethodNotFound(method);
    }

    const auto &endpoint = i->second;

    auto errors = validate(params.json, endpoint.schema.params);

    if (!errors.empty())
    {
        throw InvalidParams("Invalid params schema", errors);
    }

    return EndpointTask(endpoint, std::move(params));
}

Endpoint &EndpointRegistry::add(Endpoint endpoint)
{
    const auto &method = endpoint.schema.method;

    auto [i, inserted] = _endpoints.emplace(method, std::move(endpoint));

    if (!inserted)
    {
        throw std::invalid_argument("Duplicated endpoint method");
    }

    return i->second;
}
}
