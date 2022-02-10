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

#include "JsonRpcSender.h"

#include <brayns/common/Log.h>

#include <brayns/json/Json.h>

namespace
{
class MessageSenderHelper
{
public:
    static void trySend(const std::string &data, const brayns::ClientRef &client)
    {
        brayns::Log::trace("Sending message to client {}: '{}'.", client, data);
        try
        {
            auto &socket = client.getSocket();
            socket.send(data);
        }
        catch (const brayns::ConnectionClosedException &e)
        {
            brayns::Log::debug("Connection closed while sending data to {}: {}.", client, e.what());
        }
        catch (const std::exception &e)
        {
            brayns::Log::error("Unexpected error while sending data to {}: {}.", client, e.what());
        }
        catch (...)
        {
            brayns::Log::error("Unknown error while sending data to {}.", client);
        }
    }

    template<typename MessageType>
    static void trySend(const MessageType &message, const brayns::ClientRef &client)
    {
        auto data = brayns::Json::stringify(message);
        trySend(data, client);
    }
};
}

namespace brayns
{
void JsonRpcSender::reply(const ReplyMessage &message, const ClientRef &client)
{
    if (message.id.isEmpty())
    {
        return;
    }
    MessageSenderHelper::trySend(message, client);
}

void JsonRpcSender::error(const ErrorMessage &message, const ClientRef &client)
{
    MessageSenderHelper::trySend(message, client);
}

void JsonRpcSender::progress(const ProgressMessage &message, const ClientRef &client)
{
    if (message.params.id.isEmpty())
    {
        return;
    }
    MessageSenderHelper::trySend(message, client);
}

void JsonRpcSender::notification(const NotificationMessage &message, ClientManager &clients, const ClientRef &source)
{
    auto data = Json::stringify(message);
    clients.broadcast(data, source);
}
} // namespace brayns
