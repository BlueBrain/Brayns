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

#include <limits>

#include <brayns/json/Json.h>

#include <brayns/network/client/ClientSender.h>

#include <brayns/utils/binary/ByteConverter.h>
#include <brayns/utils/binary/ByteOrder.h>

#include "JsonRpcFactory.h"

namespace
{
class TextSender
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

class BinaryFormatter
{
public:
    static std::string format(const brayns::ReplyMessage &message, std::string_view binary)
    {
        auto json = brayns::Json::stringify(message);
        auto size = json.size();
        auto result = _formatHeader(size);
        result.append(json);
        result.append(binary);
        return result;
    }

private:
    static std::string _formatHeader(size_t size)
    {
        if (size > std::numeric_limits<uint32_t>::max())
        {
            throw brayns::InternalErrorException("JSON reply is too big: " + std::to_string(size));
        }
        auto jsonSize = static_cast<uint32_t>(size);
        return _formatJsonSize(jsonSize);
    }

    static std::string _formatJsonSize(uint32_t size)
    {
        auto bytes = brayns::ByteConverter::getBytes(size);
        auto stride = sizeof(size);
        brayns::ByteOrderHelper::convertFromSystemByteOrder(bytes, brayns::ByteOrder::LittleEndian);
        return {bytes, stride};
    }
};

class BinarySender
{
public:
    static void send(const brayns::ReplyMessage &message, std::string_view binary, const brayns::ClientRef &client)
    {
        try
        {
            _send(message, binary, client);
        }
        catch (const brayns::JsonRpcException &e)
        {
            auto error = brayns::JsonRpcFactory::error(message, e);
            TextSender::send(error, client);
        }
    }

private:
    static void _send(const brayns::ReplyMessage &message, std::string_view binary, const brayns::ClientRef &client)
    {
        auto data = BinaryFormatter::format(message, binary);
        auto packet = brayns::OutputPacket::fromBinary(data);
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
    TextSender::send(message, client);
}

void JsonRpcSender::reply(const ReplyMessage &message, std::string_view binary, const ClientRef &client)
{
    if (message.id.isEmpty())
    {
        return;
    }
    BinarySender::send(message, binary, client);
}

void JsonRpcSender::error(const ErrorMessage &message, const ClientRef &client)
{
    TextSender::send(message, client);
}

void JsonRpcSender::progress(const ProgressMessage &message, const ClientRef &client)
{
    if (message.params.id.isEmpty())
    {
        return;
    }
    TextSender::send(message, client);
}
} // namespace brayns
