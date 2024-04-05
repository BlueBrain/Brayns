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

#include <string_view>

#include <spdlog/fmt/fmt.h>

#include <brayns/core/network/client/ClientRef.h>
#include <brayns/core/network/jsonrpc/JsonRpcException.h>
#include <brayns/core/network/jsonrpc/JsonRpcMessages.h>

namespace brayns
{
/**
 * @brief Represent a JSON-RPC request sent to brayns.
 *
 * Used to store request content and send messages (reply, error, update).
 *
 */
class JsonRpcRequest
{
public:
    /**
     * @brief Construct a request from client and message.
     *
     * @param client Client that sent the message.
     * @param message Request message received from client.
     */
    JsonRpcRequest(ClientRef client, RequestMessage message, std::string binary = {});

    /**
     * @brief Return the client sending the request.
     *
     * @return const ClientRef & Client ref.
     */
    const ClientRef &getClient() const;

    /**
     * @brief Get the ID of the request from the message.
     *
     * @return const RequestId& Request ID.
     */
    const RequestId &getId() const;

    /**
     * @brief Get the method of the request (entrypoint name) from the message.
     *
     * @return const std::string& The name of the requested method.
     */
    const std::string &getMethod() const;

    /**
     * @brief Get the content of the message (parameters).
     *
     * @return const JsonValue& Message content in JSON format.
     */
    const JsonValue &getParams() const;

    /**
     * @brief Get the Binary data of the request if any.
     *
     * @return const std::string& Binary data as string.
     */
    const std::string &getBinary() const;

    /**
     * @brief Send a reply message in case of success.
     *
     * The message sent will be formatted according to the RequestMessage
     * stored in the instance.
     *
     * @param result Message content stored under "result" in the reply.
     */
    void reply(const JsonValue &result) const;

    /**
     * @brief Send a reply message with binary data in case of success.
     *
     * The message sent will be formatted according to the RequestMessage
     * stored in the instance.
     *
     * @param result Message content stored under "result" in the reply.
     * @param binary Binary data packed with JSON reply.
     */
    void reply(const JsonValue &result, std::string_view binary) const;

    /**
     * @brief Send an error message in case of exception.
     *
     * @param e Source of the error.
     */
    void error(const JsonRpcException &e) const;

    /**
     * @brief Send a progress message.
     *
     * @param operation Description of current operation.
     * @param amount Progress 0-1.
     */
    void progress(const std::string &operation, double amount) const;

private:
    ClientRef _client;
    RequestMessage _message;
    std::string _binary;
};
} // namespace brayns

namespace fmt
{
template<>
struct formatter<brayns::JsonRpcRequest> : fmt::formatter<std::string>
{
    auto format(const brayns::JsonRpcRequest &request, fmt::format_context &context) const
    {
        auto &client = request.getClient();
        auto &id = request.getId();
        auto &method = request.getMethod();
        auto &binary = request.getBinary();
        constexpr auto format = "{{client = {}, id = {}, method = {}, binary = {} bytes}}";
        return format_to(context.out(), format, client, id, method, binary.size());
    }
};
} // namespace fmt
