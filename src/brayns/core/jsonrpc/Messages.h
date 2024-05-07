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

#include <brayns/core/jsonv2/Json.h>

namespace brayns::experimental
{
using JsonRpcId = std::variant<NullJson, int, std::string>;

struct JsonRpcRequest
{
    JsonRpcId id;
    std::string method;
    JsonValue params;
    std::string binary = {};
};

template<>
struct JsonObjectReflector<JsonRpcRequest>
{
    static auto reflect()
    {
        auto builder = JsonObjectInfoBuilder<JsonRpcRequest>();
        builder.constant("jsonrpc", "2.0");
        builder.field("id", [](auto &object) { return &object.id; }).required(false);
        builder.field("method", [](auto &object) { return &object.method; });
        builder.field("params", [](auto &object) { return &object.params; }).required(false);
        return builder.build();
    }
};

struct JsonRpcResponse
{
    JsonRpcId id;
    JsonValue result;
    std::string binary = {};
};

template<>
struct JsonObjectReflector<JsonRpcResponse>
{
    static auto reflect()
    {
        auto builder = JsonObjectInfoBuilder<JsonRpcResponse>();
        builder.constant("jsonrpc", "2.0");
        builder.field("id", [](auto &object) { return &object.id; });
        builder.field("result", [](auto &object) { return &object.result; });
        return builder.build();
    }
};

struct JsonRpcError
{
    int code;
    std::string message;
    JsonValue data;
};

template<>
struct JsonObjectReflector<JsonRpcError>
{
    static auto reflect()
    {
        auto builder = JsonObjectInfoBuilder<JsonRpcError>();
        builder.field("code", [](auto &object) { return &object.code; });
        builder.field("message", [](auto &object) { return &object.message; });
        builder.field("data", [](auto &object) { return &object.data; }).required(false);
        return builder.build();
    }
};

struct JsonRpcErrorResponse
{
    JsonRpcId id;
    JsonRpcError error;
};

template<>
struct JsonObjectReflector<JsonRpcErrorResponse>
{
    static auto reflect()
    {
        auto builder = JsonObjectInfoBuilder<JsonRpcErrorResponse>();
        builder.constant("jsonrpc", "2.0");
        builder.field("id", [](auto &object) { return &object.id; });
        builder.field("error", [](auto &object) { return &object.error; });
        return builder.build();
    }
};

struct JsonRpcProgress
{
    JsonRpcId id;
    std::string operation;
    float amount;
};

template<>
struct JsonObjectReflector<JsonRpcProgress>
{
    static auto reflect()
    {
        auto builder = JsonObjectInfoBuilder<JsonRpcProgress>();
        builder.field("id", [](auto &object) { return &object.id; });
        builder.field("operation", [](auto &object) { return &object.operation; });
        builder.field("amount", [](auto &object) { return &object.amount; });
        return builder.build();
    }
};

struct JsonRpcNotification
{
    JsonRpcProgress params;
};

template<>
struct JsonObjectReflector<JsonRpcNotification>
{
    static auto reflect()
    {
        auto builder = JsonObjectInfoBuilder<JsonRpcNotification>();
        builder.constant("jsonrpc", "2.0");
        builder.field("params", [](auto &object) { return &object.params; });
        return builder.build();
    }
};
}
