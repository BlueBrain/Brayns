/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include "NetworkRequest.h"

#include <brayns/common/Log.h>

#include <brayns/network/entrypoint/EntrypointException.h>

namespace brayns
{
NetworkRequest::NetworkRequest()
{
    _setupInvalidMessage();
}

NetworkRequest::NetworkRequest(ConnectionHandle handle, ConnectionManager &connections)
    : _connection(std::move(handle), connections)
{
    _setupInvalidMessage();
}

const ConnectionHandle &NetworkRequest::getConnectionHandle() const
{
    return _connection.getHandle();
}

const RequestMessage &NetworkRequest::getMessage() const
{
    return _message;
}

const RequestId &NetworkRequest::getId() const
{
    return _message.id;
}

bool NetworkRequest::shouldBeReplied() const
{
    return !getId().isEmpty();
}

const std::string &NetworkRequest::getMethod() const
{
    return _message.method;
}

const JsonValue &NetworkRequest::getParams() const
{
    return _message.params;
}

void NetworkRequest::setMessage(RequestMessage message)
{
    _message = std::move(message);
}

void NetworkRequest::reply(const JsonValue &result) const
{
    if (!shouldBeReplied())
    {
        return;
    }
    auto reply = MessageFactory::createReply(_message);
    reply.result = result;
    _send(reply);
}

void NetworkRequest::error(int code, const std::string &message, const JsonValue &data) const
{
    if (!shouldBeReplied())
    {
        return;
    }
    _error(code, message, data);
}

void NetworkRequest::error(const std::string &message) const
{
    error(0, message);
}

void NetworkRequest::error(std::exception_ptr e) const
{
    if (!shouldBeReplied())
    {
        return;
    }
    _error(e);
}

void NetworkRequest::invalidRequest(std::exception_ptr e) const
{
    _error(e);
}

void NetworkRequest::progress(const std::string &operation, double amount) const
{
    auto progress = MessageFactory::createProgress(_message);
    progress.params.operation = operation;
    progress.params.amount = amount;
    auto packet = Json::stringify(progress);
    _connection.broadcast(packet);
}

void NetworkRequest::_setupInvalidMessage()
{
    _message.jsonrpc = "2.0";
}

void NetworkRequest::_error(int code, const std::string &message, const JsonValue &data) const
{
    auto reply = MessageFactory::createError(_message);
    auto &error = reply.error;
    error.code = code;
    error.message = message;
    error.data = data;
    _send(reply);
}

void NetworkRequest::_error(std::exception_ptr e) const
{
    if (!e)
    {
        return;
    }
    try
    {
        std::rethrow_exception(e);
    }
    catch (const EntrypointException &e)
    {
        _error(e.getCode(), e.what(), e.getData());
    }
    catch (const ConnectionClosedException &e)
    {
        Log::info("Connection closed during request processing: {}.", e.what());
    }
    catch (const std::exception &e)
    {
        _error(0, e.what(), {});
    }
    catch (...)
    {
        Log::error("Unknown error in request processing.");
        _error(0, "Unknown error", {});
    }
}
} // namespace brayns
