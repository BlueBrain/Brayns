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

#pragma once

#include <brayns/json/Json.h>

#include "RequestId.h"

namespace brayns
{
/**
 * @brief Message used to send a request to Brayns.
 *
 */
struct RequestMessage
{
    std::string jsonrpc;
    RequestId id;
    std::string method;
    JsonValue params;
};

template<>
struct JsonAdapter<RequestMessage> : ObjectAdapter<RequestMessage>
{
    static void reflect()
    {
        title("RequestMessage");
        getset(
            "jsonrpc",
            [](auto &object) -> auto & { return object.jsonrpc; },
            [](auto &object, auto value) { object.jsonrpc = std::move(value); })
            .description("JSON-RPC version");
        getset(
            "id",
            [](auto &object) -> auto & { return object.id; },
            [](auto &object, auto value) { object.id = std::move(value); })
            .description("Request ID")
            .required(false);
        getset(
            "method",
            [](auto &object) -> auto & { return object.method; },
            [](auto &object, auto value) { object.method = std::move(value); })
            .description("Entrypoint name");
        getset(
            "params",
            [](auto &object) -> auto & { return object.params; },
            [](auto &object, const auto &value) { object.params = value; })
            .description("Request content")
            .required(false);
    }
};

/**
 * @brief Request parameter to cancel an async entrypoint.
 *
 */
struct CancelParams
{
    RequestId id;
};

template<>
struct JsonAdapter<CancelParams> : ObjectAdapter<CancelParams>
{
    static void reflect()
    {
        title("CancelParams");
        getset(
            "id",
            [](auto &object) -> auto & { return object.id; },
            [](auto &object, auto value) { object.id = std::move(value); })
            .description("ID of the request to cancel");
    }
};

/**
 * @brief Message used to send a reply from Brayns.
 *
 */
struct ReplyMessage
{
    std::string jsonrpc;
    RequestId id;
    JsonValue result;
};

template<>
struct JsonAdapter<ReplyMessage> : ObjectAdapter<ReplyMessage>
{
    static void reflect()
    {
        title("ReplyMessage");
        get(
            "jsonrpc",
            [](auto &object) -> auto & { return object.jsonrpc; })
            .description("JSON-RPC version");
        get(
            "id",
            [](auto &object) -> auto & { return object.id; })
            .description("ID of the corresponding request");
        get(
            "result",
            [](auto &object) -> auto & { return object.result; })
            .description("Reply content");
    }
};

/**
 * @brief Error details when a request processing fails.
 *
 */
struct ErrorInfo
{
    int code = 0;
    std::string message;
    JsonValue data;
};

template<>
struct JsonAdapter<ErrorInfo> : ObjectAdapter<ErrorInfo>
{
    static void reflect()
    {
        title("ErrorInfo");
        get("code", [](auto &object) { return object.code; }).description("Error code");
        get(
            "message",
            [](auto &object) -> auto & { return object.message; })
            .description("Error description");
        get(
            "data",
            [](auto &object) -> auto & { return object.data; })
            .description("Additional error info");
    }
};

/**
 * @brief Message used by brayns to send an error reply.
 *
 */
struct ErrorMessage
{
    std::string jsonrpc;
    RequestId id;
    ErrorInfo error;
};

template<>
struct JsonAdapter<ErrorMessage> : ObjectAdapter<ErrorMessage>
{
    static void reflect()
    {
        title("ErrorMessage");
        get(
            "jsonrpc",
            [](auto &object) -> auto & { return object.jsonrpc; })
            .description("JSON-RPC version");
        get(
            "id",
            [](auto &object) -> auto & { return object.id; })
            .description("ID of the corresponding request");
        get(
            "error",
            [](auto &object) -> auto & { return object.error; })
            .description("Error info");
    }
};

/**
 * @brief Progress info used in progress messages.
 *
 */
struct ProgressInfo
{
    RequestId id;
    std::string operation;
    double amount = 0.0;
};

template<>
struct JsonAdapter<ProgressInfo> : ObjectAdapter<ProgressInfo>
{
    static void reflect()
    {
        title("ProgressInfo");
        get(
            "id",
            [](auto &object) -> auto & { return object.id; })
            .description("ID of the corresponding request");
        get(
            "operation",
            [](auto &object) -> auto & { return object.operation; })
            .description("Description of the current task");
        get("amount", [](auto &object) { return object.amount; }).description("Global progess [0-1]");
    }
};

/**
 * @brief Progress message sent during a request processing.
 *
 */
struct ProgressMessage
{
    std::string jsonrpc;
    std::string method;
    ProgressInfo params;
};

template<>
struct JsonAdapter<ProgressMessage> : ObjectAdapter<ProgressMessage>
{
    static void reflect()
    {
        title("ProgressMessage");
        get(
            "jsonrpc",
            [](auto &object) -> auto & { return object.jsonrpc; })
            .description("JSON-RPC version");
        get(
            "method",
            [](auto &object) -> auto & { return object.method; })
            .description("Entrypoint name");
        get(
            "params",
            [](auto &object) -> auto & { return object.params; })
            .description("Progress info");
    }
};
} // namespace brayns
