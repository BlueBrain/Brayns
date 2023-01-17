/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#include "ClientSender.h"

#include <brayns/utils/binary/ByteConverter.h>
#include <brayns/utils/binary/ByteOrder.h>

namespace
{
class BinaryFormatter
{
public:
    static std::string format(std::string_view text, std::string_view binary)
    {
        auto size = text.size();
        auto result = _formatHeader(size);
        result.append(text);
        result.append(binary);
        return result;
    }

private:
    static std::string _formatHeader(size_t size)
    {
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
} // namespace

namespace brayns
{
void ClientSender::sendRawBinary(std::string_view text, std::string_view binary, const ClientRef &client)
{
    auto data = BinaryFormatter::format(text, binary);
    auto packet = brayns::OutputPacket::fromBinary(data);
    client.send(packet);
}

void ClientSender::sendRawText(std::string_view text, const ClientRef &client)
{
    auto packet = brayns::OutputPacket::fromText(text);
    client.send(packet);
}
} // namespace brayns
