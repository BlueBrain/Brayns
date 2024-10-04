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
#include <string>

#include <brayns/core/json/Json.h>
#include <brayns/core/jsonrpc/Messages.h>

#include "Progress.h"

namespace brayns
{
struct EndpointSchema
{
    std::string method;
    std::string description = {};
    JsonSchema params;
    JsonSchema result;
};

template<>
struct JsonObjectReflector<EndpointSchema>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<EndpointSchema>();
        builder.field("method", [](auto &object) { return &object.method; })
            .description("JSON-RPC method that has to be specified to reach the endpoint");
        builder.field("description", [](auto &object) { return &object.description; }).description("Short description of what the method does");
        builder.field("params", [](auto &object) { return &object.params; }).description("JSON schema of the method params");
        builder.field("result", [](auto &object) { return &object.result; }).description("JSON schema of the method result");
        return builder.build();
    }
};

using EndpointHandler = std::function<Payload(Payload, Progress)>;

struct Endpoint
{
    EndpointSchema schema;
    EndpointHandler handler;
};
}
