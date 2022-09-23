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

#include "ClientRequest.h"

#include <brayns/network/jsonrpc/JsonRpcFactory.h>
#include <brayns/network/jsonrpc/JsonRpcSender.h>

namespace brayns
{
ClientRequest::ClientRequest(ClientRef client, InputPacket packet)
    : _client(std::move(client))
    , _packet(std::move(packet))
{
}

const ClientRef &ClientRequest::getClient() const
{
    return _client;
}

bool ClientRequest::isBinary() const
{
    return _packet.isBinary();
}

bool ClientRequest::isText() const
{
    return _packet.isText();
}

std::string_view ClientRequest::getData() const
{
    return _packet.getData();
}

void ClientRequest::error(const JsonRpcException &e) const
{
    auto message = JsonRpcFactory::error(e);
    JsonRpcSender::error(message, _client);
}
} // namespace brayns

namespace std
{
std::ostream &operator<<(std::ostream &stream, const brayns::ClientRequest &request)
{
    auto &client = request.getClient();
    auto data = request.getData();
    auto size = data.size();
    auto binary = request.isBinary() ? "true" : "false";
    stream << "{client = " << client;
    stream << ", size = " << size;
    stream << ", binary = " << binary << "}";
    return stream;
}
} // namespace std
