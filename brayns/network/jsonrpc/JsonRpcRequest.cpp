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

#include "JsonRpcRequest.h"

#include <brayns/json/Json.h>

#include <brayns/network/client/ClientSender.h>
#include <brayns/network/jsonrpc/JsonRpcFactory.h>

namespace brayns
{
JsonRpcRequest::JsonRpcRequest(ClientRef client, RequestMessage message, std::string binary):
    _client(std::move(client)),
    _message(std::move(message)),
    _binary(std::move(binary))
{
}

const ClientRef &JsonRpcRequest::getClient() const
{
    return _client;
}

const RequestId &JsonRpcRequest::getId() const
{
    return _message.id;
}

const std::string &JsonRpcRequest::getMethod() const
{
    return _message.method;
}

const JsonValue &JsonRpcRequest::getParams() const
{
    return _message.params;
}

const std::string &JsonRpcRequest::getBinary() const
{
    return _binary;
}

void JsonRpcRequest::reply(const JsonValue &result) const
{
    if (_message.id.isEmpty())
    {
        return;
    }
    auto message = JsonRpcFactory::reply(_message, result);
    ClientSender::sendText(message, _client);
}

void JsonRpcRequest::reply(const JsonValue &result, std::string_view binary) const
{
    if (_message.id.isEmpty())
    {
        return;
    }
    auto message = JsonRpcFactory::reply(_message, result);
    ClientSender::sendBinary(message, binary, _client);
}

void JsonRpcRequest::error(const JsonRpcException &e) const
{
    if (_message.id.isEmpty())
    {
        return;
    }
    auto message = brayns::JsonRpcFactory::error(_message, e);
    brayns::ClientSender::sendText(message, _client);
}

void JsonRpcRequest::progress(const std::string &operation, double amount) const
{
    if (_message.id.isEmpty())
    {
        return;
    }
    auto message = JsonRpcFactory::progress(_message, operation, amount);
    ClientSender::sendText(message, _client);
}
} // namespace brayns
