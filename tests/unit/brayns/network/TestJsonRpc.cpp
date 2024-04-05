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

#include <doctest/doctest.h>

#include <brayns/core/network/jsonrpc/JsonRpcParser.h>

#include "MockWebSocket.h"

TEST_CASE("JsonRpcParser")
{
    auto socket = std::make_shared<MockWebSocket>();
    auto client = brayns::ClientRef(socket);
    auto json = std::string(R"({
        "jsonrpc": "2.0",
        "id": 1,
        "method": "test",
        "params": 123
    })");

    SUBCASE("Text")
    {
        auto raw = brayns::ClientRequest(client, brayns::InputPacket::fromText(json));
        auto request = brayns::JsonRpcParser::parse(raw);
        CHECK_EQ(request.getClient(), client);
        CHECK_EQ(request.getId().toInt(), 1);
        CHECK_EQ(request.getMethod(), "test");
        CHECK_EQ(request.getParams(), 123);
    }
    SUBCASE("Binary")
    {
        auto data = std::string("\x62\x00\x00\x00", 4) + json + "binary";
        auto raw = brayns::ClientRequest(client, brayns::InputPacket::fromBinary(data));
        auto request = brayns::JsonRpcParser::parse(raw);
        CHECK_EQ(request.getClient(), client);
        CHECK_EQ(request.getId().toInt(), 1);
        CHECK_EQ(request.getMethod(), "test");
        CHECK_EQ(request.getParams(), 123);
        CHECK_EQ(request.getBinary(), "binary");
    }
    SUBCASE("Invalid JSON")
    {
        auto data = "{\"test";
        auto raw = brayns::ClientRequest(client, brayns::InputPacket::fromText(data));
        CHECK_THROWS_AS(brayns::JsonRpcParser::parse(raw), brayns::ParsingErrorException);
    }
    SUBCASE("Invalid schema")
    {
        auto data = R"({
            "id": 1,
            "method": "test",
            "params": 123
        })";
        auto raw = brayns::ClientRequest(client, brayns::InputPacket::fromText(data));
        CHECK_THROWS_AS(brayns::JsonRpcParser::parse(raw), brayns::InvalidRequestException);
    }
    SUBCASE("Invalid JSON-RPC version")
    {
        auto data = R"({
            "jsonrpc": "1.0",
            "id": 1,
            "method": "test",
            "params": 123
        })";
        auto raw = brayns::ClientRequest(client, brayns::InputPacket::fromText(data));
        CHECK_THROWS_AS(brayns::JsonRpcParser::parse(raw), brayns::InvalidRequestException);
    }
    SUBCASE("No methods")
    {
        auto data = R"({
            "jsonrpc": "2.0",
            "id": 1,
            "params": 123
        })";
        auto raw = brayns::ClientRequest(client, brayns::InputPacket::fromText(data));
        CHECK_THROWS_AS(brayns::JsonRpcParser::parse(raw), brayns::InvalidRequestException);
    }
    SUBCASE("Binary without size")
    {
        auto data = "123";
        auto raw = brayns::ClientRequest(client, brayns::InputPacket::fromBinary(data));
        CHECK_THROWS_AS(brayns::JsonRpcParser::parse(raw), brayns::ParsingErrorException);
    }
    SUBCASE("Binary size bigger than frame size")
    {
        auto data = std::string("\x10\x00\x00\x00", 4) + "{}";
        auto raw = brayns::ClientRequest(client, brayns::InputPacket::fromBinary(data));
        CHECK_THROWS_AS(brayns::JsonRpcParser::parse(raw), brayns::ParsingErrorException);
    }
}

TEST_CASE("JsonRpcRequest")
{
    auto id = 3;
    auto socket = std::make_shared<MockWebSocket>(id);
    auto client = brayns::ClientRef(socket);
    auto message = brayns::RequestMessage();
    message.id = brayns::RequestId(1);
    message.method = "test";
    auto binary = "binary";
    auto request = brayns::JsonRpcRequest(client, message, binary);
    auto test = fmt::format("{}", request);
    auto ref = "{client = 3, id = 1, method = test, binary = 6 bytes}";
    CHECK_EQ(test, ref);
}
