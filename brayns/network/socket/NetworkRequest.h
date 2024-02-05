/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/common/Log.h>

#include <brayns/network/entrypoint/EntrypointException.h>
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
    NetworkRequest() { _setupInvalidMessage(); }

    /**
     * @brief Construct a request from a connection handle and the connection
     * manager used to send packets.
     *
     * @param handle Connection handle.
     * @param manager Connection manager.
     */
    NetworkRequest(ConnectionHandle handle, ConnectionManager& connections)
        : _connection(std::move(handle), connections)
    {
        _setupInvalidMessage();
    }

    /**
     * @brief Return the connection handle associated with the request.
     *
     * @return const ConnectionHandle& Connection handle with the client.
     */
    const ConnectionHandle& getConnectionHandle() const
    {
        return _connection.getHandle();
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
     * @return const RequestId& Request ID.
     */
    const RequestId& getId() const { return _message.id; }

    /**
     * @brief Check if the request expects a reply.
     *
     * @return true A reply must be send (even with no result).
     * @return false No reply can be sent.
     */
    bool shouldBeReplied() const { return !getId().isEmpty(); }

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
        if (!shouldBeReplied())
        {
            return;
        }
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
    void error(int code, const std::string& message,
               const JsonValue& data = {}) const
    {
        if (!shouldBeReplied())
        {
            return;
        }
        _error(code, message, data);
    }

    /**
     * @brief Shortcut for errors with no code.
     *
     * @param message Error message.
     */
    void error(const std::string& message) const { error(0, message); }

    /**
     * @brief Shortcut to process an arbitrary exception.
     *
     * @param e Opaque exception pointer.
     */
    void error(std::exception_ptr e) const
    {
        if (!shouldBeReplied())
        {
            return;
        }
        _error(e);
    }

    /**
     * @brief Report an error even if ID is null.
     *
     * @param e Opaque exception pointer.
     */
    void invalidRequest(std::exception_ptr e) const { _error(e); }

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
        _connection.broadcast(packet);
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
    template <typename MessageType>
    void notify(const MessageType& params) const
    {
        NotificationMessage notification;
        notification.jsonrpc = "2.0";
        notification.method = getMethod();
        notification.params = Json::serialize(params);
        auto json = Json::stringify(notification);
        _connection.broadcastToOtherClients(json);
    }

private:
    void _setupInvalidMessage() { _message.jsonrpc = "2.0"; }

    void _error(int code, const std::string& message,
                const JsonValue& data) const
    {
        auto reply = MessageFactory::createError(_message);
        auto& error = reply.error;
        error.code = code;
        error.message = message;
        error.data = data;
        _send(reply);
    }

    void _error(std::exception_ptr e) const
    {
        if (!e)
        {
            return;
        }
        try
        {
            std::rethrow_exception(e);
        }
        catch (const EntrypointException& e)
        {
            _error(e.getCode(), e.what(), e.getData());
        }
        catch (const ConnectionClosedException& e)
        {
            Log::info("Connection closed during request processing: {}.",
                      e.what());
        }
        catch (const std::exception& e)
        {
            _error(0, e.what(), {});
        }
        catch (...)
        {
            Log::error("Unknown error in request processing.");
            _error(0, "Unknown error", {});
        }
    }

    template <typename MessageType>
    void _send(const MessageType& message) const
    {
        auto packet = Json::stringify(message);
        _connection.send(packet);
    }

    ConnectionRef _connection;
    RequestMessage _message;
};
} // namespace brayns