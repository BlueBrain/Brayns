/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <memory>

#include <doctest.h>

#include <brayns/core/network/client/ClientRef.h>
#include <brayns/core/network/client/ClientRequest.h>
#include <brayns/core/network/client/ClientSender.h>
#include <brayns/core/network/websocket/IWebSocket.h>

#include "MockWebSocket.h"

TEST_CASE("ClientSender")
{
    SUBCASE("binary")
    {
        auto socket = std::make_shared<MockWebSocket>();
        auto client = brayns::ClientRef(socket);
        brayns::ClientSender::sendRawBinary("text", "binary", client);
        auto &received = socket->getReceivedPackets();
        CHECK_EQ(received.size(), 1);
        auto &packet = received.front();
        CHECK(packet.binary);
        auto data = std::string_view("\x04\x00\x00\x00textbinary", 14);
        CHECK_EQ(packet.data, data);
    }
    SUBCASE("text")
    {
        auto data = std::string_view("test");
        auto socket = std::make_shared<MockWebSocket>();
        auto client = brayns::ClientRef(socket);
        brayns::ClientSender::sendRawText(data, client);
        auto &received = socket->getReceivedPackets();
        CHECK_EQ(received.size(), 1);
        auto &packet = received.front();
        CHECK_FALSE(packet.binary);
        CHECK_EQ(packet.data, data);
    }
}

TEST_CASE("ClientRequest")
{
    auto socket = std::make_shared<MockWebSocket>(3);
    auto client = brayns::ClientRef(socket);
    auto packet = brayns::InputPacket::fromText("test");
    auto request = brayns::ClientRequest(client, packet);
    auto test = fmt::format("{}", request);
    auto ref = "{client = 3, size = 4, binary = false}";
    CHECK_EQ(test, ref);
}
