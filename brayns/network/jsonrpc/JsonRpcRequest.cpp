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

#include "JsonRpcRequest.h"

#include <brayns/json/Json.h>

#include <brayns/network/jsonrpc/JsonRpcFactory.h>
#include <brayns/network/jsonrpc/JsonRpcSender.h>

namespace brayns
{
JsonRpcRequest::JsonRpcRequest(ClientRef client, RequestMessage message, std::string binary)
    : _client(std::move(client))
    , _message(std::move(message))
    , _binary(std::move(binary))
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
    auto message = JsonRpcFactory::reply(_message, result);
    JsonRpcSender::reply(message, _client);
}

void JsonRpcRequest::reply(const JsonValue &result, std::string_view binary) const
{
    auto message = JsonRpcFactory::reply(_message, result);
    JsonRpcSender::reply(message, binary, _client);
}

void JsonRpcRequest::error(const JsonRpcException &e) const
{
    auto message = brayns::JsonRpcFactory::error(_message, e);
    brayns::JsonRpcSender::error(message, _client);
}

void JsonRpcRequest::progress(const std::string &operation, double amount) const
{
    auto message = JsonRpcFactory::progress(_message, operation, amount);
    JsonRpcSender::progress(message, _client);
}
} // namespace brayns

namespace std
{
std::ostream &operator<<(std::ostream &stream, const brayns::JsonRpcRequest &request)
{
    auto &client = request.getClient();
    auto &id = request.getId();
    auto &method = request.getMethod();
    auto &binary = request.getBinary();
    stream << "{client = " << client;
    stream << ", id = " << id;
    stream << ", method = " << method;
    stream << ", binary = " << binary.size() << " bytes}";
    return stream;
}
} // namespace std
