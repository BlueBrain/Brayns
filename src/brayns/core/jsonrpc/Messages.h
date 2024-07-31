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

#include <optional>
#include <string>
#include <variant>

#include <fmt/format.h>

#include <brayns/core/json/Json.h>

namespace brayns
{
using JsonRpcId = std::variant<int, std::string>;

inline std::string toString(const JsonRpcId &id)
{
    if (const auto *value = std::get_if<int>(&id))
    {
        return std::to_string(*value);
    }

    return fmt::format("'{}'", std::get<std::string>(id));
}

inline std::string toString(const std::optional<JsonRpcId> &id)
{
    return id ? toString(*id) : "null";
}

struct JsonRpcRequest
{
    std::string method;
    JsonValue params;
    std::string binary = {};
    std::optional<JsonRpcId> id = {};
};

template<>
struct JsonObjectReflector<JsonRpcRequest>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<JsonRpcRequest>();
        builder.constant("jsonrpc", "2.0");
        builder.field("method", [](auto &object) { return &object.method; });
        builder.field("params", [](auto &object) { return &object.params; }).required(false);
        builder.field("id", [](auto &object) { return &object.id; });
        return builder.build();
    }
};

struct JsonRpcSuccessResponse
{
    JsonRpcId id;
    JsonValue result;
    std::string binary = {};
};

template<>
struct JsonObjectReflector<JsonRpcSuccessResponse>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<JsonRpcSuccessResponse>();
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
        auto builder = JsonBuilder<JsonRpcError>();
        builder.field("code", [](auto &object) { return &object.code; });
        builder.field("message", [](auto &object) { return &object.message; });
        builder.field("data", [](auto &object) { return &object.data; }).required(false);
        return builder.build();
    }
};

struct JsonRpcErrorResponse
{
    JsonRpcError error;
    std::optional<JsonRpcId> id = {};
};

template<>
struct JsonObjectReflector<JsonRpcErrorResponse>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<JsonRpcErrorResponse>();
        builder.constant("jsonrpc", "2.0");
        builder.field("error", [](auto &object) { return &object.error; });
        builder.field("id", [](auto &object) { return &object.id; });
        return builder.build();
    }
};

using JsonRpcResponse = std::variant<JsonRpcSuccessResponse, JsonRpcErrorResponse>;
}
