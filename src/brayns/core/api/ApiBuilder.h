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

#pragma once

#include <string>
#include <utility>

#include <brayns/core/jsonv2/Json.h>

#include "Endpoint.h"
#include "Task.h"

namespace brayns::experimental
{
class EndpointBuilder
{
public:
    explicit EndpointBuilder(Endpoint &endpoint):
        _endpoint(&endpoint)
    {
    }

    EndpointBuilder description(std::string value)
    {
        _endpoint->schema.description = std::move(value);
        return *this;
    }

private:
    Endpoint *_endpoint;
};

class ApiBuilder
{
public:
    template<TaskLauncher T>
    EndpointBuilder task(std::string method, T launcher)
    {
        auto schema = reflectEndpointSchema<T>(std::move(method));
        auto startTask = addParsingToTaskLauncher(std::move(launcher));
        auto &endpoint = add({std::move(schema), std::move(startTask)});
        return EndpointBuilder(endpoint);
    }

    template<TaskHandler T>
    EndpointBuilder endpoint(std::string method, T handler)
    {
        auto schema = reflectEndpointSchema<T>(std::move(method));
        auto runTask = addParsingToTaskHandler(std::move(handler));
        auto &endpoint = add({std::move(schema), std::move(runTask)});
        return EndpointBuilder(endpoint);
    }

    EndpointRegistry build()
    {
        return EndpointRegistry(std::exchange(_endpoints, {}));
    }

private:
    std::map<std::string, Endpoint> _endpoints;

    Endpoint &add(Endpoint endpoint)
    {
        const auto &method = endpoint.schema.method;

        if (_endpoints.contains(method))
        {
            throw std::invalid_argument("Duplicated endpoint method");
        }

        return _endpoints[method] = std::move(endpoint);
    }
};
}
