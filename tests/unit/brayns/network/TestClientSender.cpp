/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <doctest/doctest.h>

#include <brayns/network/client/ClientRef.h>
#include <brayns/network/client/ClientSender.h>
#include <brayns/network/websocket/IWebSocket.h>

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
        auto data = std::string_view("\004\000\000\000textbinary", 14);
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
