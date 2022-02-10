/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/network/client/ClientRef.h>
#include <brayns/network/jsonrpc/JsonRpcMessages.h>

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
    JsonRpcRequest(ClientRef client, RequestMessage message);

    /**
     * @brief Return the client sending the request.
     *
     * @return const ClientRef & Client ref.
     */
    const ClientRef &getClient() const;

    /**
     * @brief Get the entire JSON-RPC message.
     *
     * @return const RequestMessage& The message sent by the client.
     */
    const RequestMessage &getMessage() const;

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
     * @brief Send a reply message in case of success.
     *
     * The message sent will be formatted according to the RequestMessage
     * stored in the instance.
     *
     * @param result Message content stored under "result" in the reply.
     */
    void reply(const JsonValue &result) const;

    /**
     * @brief Send a progress message.
     *
     * @param amount Progress 0-1.
     * @param operation Description of current operation.
     */
    void progress(double amount, const std::string &operation) const;

private:
    ClientRef _client;
    RequestMessage _message;
};
} // namespace brayns
