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

#include <functional>
#include <map>
#include <string>
#include <string_view>

#include <brayns/core/jsonv2/Json.h>

namespace brayns::experimental
{
struct EndpointSchema
{
    std::string method;
    std::string description = {};
    JsonSchema params;
    JsonSchema result;
};

template<>
struct JsonReflector<EndpointSchema>
{
    static auto reflect()
    {
        auto builder = JsonObjectInfoBuilder<EndpointSchema>();
        builder.field("method", [](auto &object) { return &object.method; }).description("Endpoint method");
        builder.field("description", [](auto &object) { return &object.method; }).description("Endpoint description");
        builder.field("params", [](auto &object) { return &object.params; }).description("Endpoint params JSON schema");
        builder.field("result", [](auto &object) { return &object.result; }).description("Endpoint result JSON schema");
        return builder.build();
    }
};

struct RawParams
{
    JsonValue json;
    std::string binary = {};
};

struct RawResult
{
    JsonValue json;
    std::string binary = {};
};

struct Endpoint
{
    EndpointSchema schema;
    std::function<RawResult(RawParams)> run;
};

class EndpointRegistry
{
public:
    explicit EndpointRegistry(std::map<std::string, Endpoint> endpoints);

    std::vector<std::string> getMethods() const;
    const Endpoint *find(const std::string &method) const;

private:
    std::map<std::string, Endpoint> _endpoints;
};
}
