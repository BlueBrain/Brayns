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

#include "MessageFactory.h"
#include "NetworkSocket.h"

namespace brayns
{
class NetworkRequest
{
public:
    NetworkRequest(NetworkSocket& socket)
        : _socket(&socket)
    {
    }

    const RequestMessage& getMessage() const { return _message; }

    const std::string& getId() const { return _message.id; }

    const std::string& getMethod() const { return _message.method; }

    const JsonValue& getParams() const { return _message.params; }

    void setMessage(RequestMessage message) { _message = std::move(message); }

    void sendReply(const ReplyMessage& reply) const { _send(reply); }

    void sendError(int code, const std::string& message) const
    {
        auto error = MessageFactory::createErrorMessage(_message);
        error.error.code = code;
        error.error.message = message;
        _send(error);
    }

    void sendProgress(const std::string& operation, double amount) const
    {
        auto progress = MessageFactory::createProgressMessage(_message);
        progress.params.operation = operation;
        progress.params.amount = amount;
        _send(progress);
    }

    template <typename MessageType>
    void sendReply(const MessageType& message) const
    {
        auto reply = MessageFactory::createReplyMessage(_message);
        reply.result = Json::serialize(message);
        _send(reply);
    }

private:
    template <typename MessageType>
    void _send(const MessageType& message) const
    {
        _socket->send(Json::stringify(message));
    }

    RequestMessage _message;
    NetworkSocket* _socket;
};
} // namespace brayns