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

#include <string>

#include <brayns/network/json/MessageFactory.h>

#include "ConnectionRef.h"

namespace brayns
{
/**
 * @brief Represent a client request sent to brayns.
 *
 * Used to store request content and send messages (reply, error, update).
 *
 */
class NetworkRequest
{
public:
    /**
     * @brief Construct an invalid request.
     *
     */
    NetworkRequest();

    /**
     * @brief Construct a request from a connection handle and the connection
     * manager used to send packets.
     *
     * @param handle Connection handle.
     * @param manager Connection manager.
     */
    NetworkRequest(ConnectionHandle handle, ConnectionManager &connections);

    /**
     * @brief Return the connection handle associated with the request.
     *
     * @return const ConnectionHandle& Connection handle with the client.
     */
    const ConnectionHandle &getConnectionHandle() const;

    /**
     * @brief Get the message of the client request.
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
     * @brief Check if the request expects a reply.
     *
     * @return true A reply must be send (even with no result).
     * @return false No reply can be sent.
     */
    bool shouldBeReplied() const;

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
     * @brief Setup the client message.
     *
     * @param message Client message.
     */
    void setMessage(RequestMessage message);

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
     * @brief Send an error message to the client.
     *
     * Used to provide error description to the client in case of failure.
     *
     * @param code Error code.
     * @param message Error description.
     */
    void error(int code, const std::string &message, const JsonValue &data = {}) const;

    /**
     * @brief Shortcut for errors with no code.
     *
     * @param message Error message.
     */
    void error(const std::string &message) const;

    /**
     * @brief Shortcut to process an arbitrary exception.
     *
     * @param e Opaque exception pointer.
     */
    void error(std::exception_ptr e) const;

    /**
     * @brief Report an error even if ID is null.
     *
     * @param e Opaque exception pointer.
     */
    void invalidRequest(std::exception_ptr e) const;

    /**
     * @brief Send a progress message to all clients.
     *
     * Used to provide feedback during the request processing.
     *
     * @param operation Current step description.
     * @param amount Completion percentage.
     */
    void progress(const std::string &operation, double amount) const;

    /**
     * @brief Send a reply message in case of success.
     *
     * The ReplyMessage sent will be formatted according to the RequestMessage
     * stored in the instance.
     *
     * @tparam MessageType Type of the message stored in the "result" field of
     * the reply.
     * @param result Message content stored under "result" in the reply.
     */
    template<typename MessageType>
    void reply(const MessageType &result) const
    {
        if (!shouldBeReplied())
        {
            return;
        }
        reply(Json::serialize(result));
    }

    /**
     * @brief Send a notification to all other clients (not the request sender).
     *
     * @tparam MessageType Params object type.
     * @param params Params content.
     */
    template<typename MessageType>
    void notify(const MessageType &params) const
    {
        NotificationMessage notification;
        notification.jsonrpc = "2.0";
        notification.method = getMethod();
        notification.params = Json::serialize(params);
        auto json = Json::stringify(notification);
        _connection.broadcastToOtherClients(json);
    }

private:
    void _setupInvalidMessage();
    void _error(int code, const std::string &message, const JsonValue &data) const;
    void _error(std::exception_ptr e) const;

    template<typename MessageType>
    void _send(const MessageType &message) const
    {
        auto packet = Json::stringify(message);
        _connection.send(packet);
    }

    ConnectionRef _connection;
    RequestMessage _message;
};
} // namespace brayns
