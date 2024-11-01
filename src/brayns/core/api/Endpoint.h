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

#include <brayns/core/json/Json.h>
#include <brayns/core/jsonrpc/Messages.h>

#include "Task.h"

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
        builder.field("method", [](auto &object) { return &object.method; }).description("JSON-RPC method to call the endpoint");
        builder.field("description", [](auto &object) { return &object.description; }).description("Short description of what the method does");
        builder.field("params", [](auto &object) { return &object.params; }).description("JSON schema of the method params");
        builder.field("result", [](auto &object) { return &object.result; }).description("JSON schema of the method result");
        return builder.build();
    }
};

struct Endpoint
{
    EndpointSchema schema;
    std::function<Task()> start;
    bool priority = false;
};

class EndpointTask
{
public:
    explicit EndpointTask(Payload params, Task task, bool priority);

    bool hasPriority() const;
    Payload run();
    TaskOperation getCurrentOperation() const;
    void cancel();

private:
    Payload _params;
    Task _task;
    bool _priority;
};

class EndpointRegistry
{
public:
    std::vector<std::string> getMethods() const;
    const EndpointSchema &getSchema(const std::string &method) const;
    EndpointTask start(const std::string &method, Payload params) const;
    Endpoint &add(Endpoint endpoint);

private:
    std::map<std::string, Endpoint> _endpoints;
};
}
