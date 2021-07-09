/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/network/message/MessageFactory.h>

#include "ConnectionManager.h"

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
     * @brief Construct a request from the socket opened with the client.
     *
     * @param socket The socket opened by the client using HTTP.
     */
    NetworkRequest(ConnectionId id, ConnectionManager& manager)
        : _id(id)
        , _manager(&manager)
    {
    }

    /**
     * @brief Get the message of the client request.
     *
     * @return const RequestMessage& The message sent by the client.
     */
    const RequestMessage& getMessage() const { return _message; }

    /**
     * @brief Get the ID of the request from the message.
     *
     * @return const std::string& Request ID.
     */
    const std::string& getId() const { return _message.id; }

    /**
     * @brief Get the method of the request (entrypoint name) from the message.
     *
     * @return const std::string& The name of the requested method.
     */
    const std::string& getMethod() const { return _message.method; }

    /**
     * @brief Get the content of the message (parameters).
     *
     * @return const JsonValue& Message content in JSON format.
     */
    const JsonValue& getParams() const { return _message.params; }

    /**
     * @brief Setup the client message.
     *
     * @param message Client message.
     */
    void setMessage(RequestMessage message) { _message = std::move(message); }

    /**
     * @brief Send a reply message in case of success.
     *
     * The message sent will be formatted according to the RequestMessage
     * stored in the instance.
     *
     * @param result Message content stored under "result" in the reply.
     */
    void reply(const JsonValue& result) const
    {
        auto reply = MessageFactory::createReply(_message);
        reply.result = result;
        _send(reply);
    }

    /**
     * @brief Send an error message to the client.
     *
     * Used to provide error description to the client in case of failure.
     *
     * @param code Error code.
     * @param message Error description.
     */
    void error(int code, const std::string& message) const
    {
        auto error = MessageFactory::createError(_message);
        error.error.code = code;
        error.error.message = message;
        _send(error);
    }

    /**
     * @brief Shortcut for errors with no code.
     *
     * @param message Error message.
     */
    void error(const std::string& message) const { error(0, message); }

    /**
     * @brief Send a progress message to all clients.
     *
     * Used to provide feedback during the request processing.
     *
     * @param operation Current step description.
     * @param amount Completion percentage.
     */
    void progress(const std::string& operation, double amount) const
    {
        auto progress = MessageFactory::createProgress(_message);
        progress.params.operation = operation;
        progress.params.amount = amount;
        auto packet = Json::stringify(progress);
        _manager->broadcast(std::move(packet));
    }

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
    template <typename MessageType>
    void reply(const MessageType& result) const
    {
        reply(Json::serialize(result));
    }

private:
    template <typename MessageType>
    void _send(const MessageType& message) const
    {
        auto packet = Json::stringify(message);
        _manager->send(_id, packet);
    }

    ConnectionId _id;
    ConnectionManager* _manager;
    RequestMessage _message;
};
} // namespace brayns