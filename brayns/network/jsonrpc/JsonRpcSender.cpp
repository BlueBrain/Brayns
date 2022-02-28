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

#include <brayns/json/Json.h>

#include <brayns/network/client/ClientSender.h>

namespace
{
class MessageSenderHelper
{
public:
    template<typename MessageType>
    static void send(const MessageType &message, const brayns::ClientRef &client)
    {
        auto json = brayns::Json::stringify(message);
        auto packet = brayns::OutputPacket::fromText(json);
        brayns::ClientSender::send(packet, client);
    }
};
} // namespace

namespace brayns
{
void JsonRpcSender::reply(const ReplyMessage &message, const ClientRef &client)
{
    if (message.id.isEmpty())
    {
        return;
    }
    MessageSenderHelper::send(message, client);
}

void JsonRpcSender::error(const ErrorMessage &message, const ClientRef &client)
{
    MessageSenderHelper::send(message, client);
}

void JsonRpcSender::progress(const ProgressMessage &message, const ClientRef &client)
{
    if (message.params.id.isEmpty())
    {
        return;
    }
    MessageSenderHelper::send(message, client);
}
} // namespace brayns
