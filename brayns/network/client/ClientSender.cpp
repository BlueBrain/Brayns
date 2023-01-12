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

#include "ClientSender.h"

#include <brayns/utils/Log.h>

namespace
{
class MessageLogger
{
public:
    static void log(const brayns::OutputPacket &packet, const brayns::ClientRef &client)
    {
        if (packet.isBinary())
        {
            _logBinary(packet, client);
            return;
        }
        if (packet.isText())
        {
            _logText(packet, client);
            return;
        }
        brayns::Log::error("Trying to send invalid packet.");
    }

private:
    static void _logBinary(const brayns::OutputPacket &packet, const brayns::ClientRef &client)
    {
        auto data = packet.getData();
        auto size = data.size();
        brayns::Log::debug("Send binary frame of {} bytes to client {}.", size, client);
    }

    static void _logText(const brayns::OutputPacket &packet, const brayns::ClientRef &client)
    {
        auto data = packet.getData();
        brayns::Log::debug("Send text frame to client {}: '{}'.", client, data);
    }
};
} // namespace

namespace brayns
{
void ClientSender::send(const OutputPacket &packet, const ClientRef &client)
{
    MessageLogger::log(packet, client);
    try
    {
        client.send(packet);
    }
    catch (const brayns::ConnectionClosedException &e)
    {
        brayns::Log::debug("Connection closed while sending data: '{}'.", e.what());
    }
    catch (const std::exception &e)
    {
        brayns::Log::error("Unexpected error while sending data: '{}'.", e.what());
    }
    catch (...)
    {
        brayns::Log::error("Unknown error while sending data.");
    }
}
} // namespace brayns
