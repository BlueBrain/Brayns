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

#include <doctest.h>

#include <brayns/core/jsonrpc/Errors.h>
#include <brayns/core/jsonrpc/Messages.h>
#include <brayns/core/jsonrpc/Parser.h>
#include <brayns/core/utils/Binary.h>

using namespace brayns::experimental;

TEST_CASE("JsonRpcParser")
{
    auto json = std::string(R"({
        "jsonrpc": "2.0",
        "id": 1,
        "method": "test",
        "params": 123
    })");

    SUBCASE("Text")
    {
        auto request = parseJsonRpcRequest(json);

        CHECK_EQ(std::get<int>(request.id), 1);
        CHECK_EQ(request.method, "test");
        CHECK_EQ(request.params, 123);
        CHECK_EQ(request.binary, "");
    }
    SUBCASE("Binary")
    {
        auto jsonSize = static_cast<std::uint32_t>(json.size());
        auto header = composeBytes(jsonSize, std::endian::little);
        auto data = header + json + "binary";

        auto request = parseBinaryJsonRpcRequest(data);

        CHECK_EQ(std::get<int>(request.id), 1);
        CHECK_EQ(request.method, "test");
        CHECK_EQ(request.params, 123);
        CHECK_EQ(request.binary, "binary");
    }
    SUBCASE("Invalid JSON")
    {
        auto data = "{\"test";
        CHECK_THROWS_AS(parseJsonRpcRequest(data), ParseError);
    }
    SUBCASE("Invalid schema")
    {
        auto data = R"({
            "id": 1,
            "method": "test",
            "params": 123
        })";
        CHECK_THROWS_AS(parseJsonRpcRequest(data), InvalidRequest);
    }
    SUBCASE("Invalid JSON-RPC version")
    {
        auto data = R"({
            "jsonrpc": "1.0",
            "id": 1,
            "method": "test",
            "params": 123
        })";
        CHECK_THROWS_AS(parseJsonRpcRequest(data), InvalidRequest);
    }
    SUBCASE("No methods")
    {
        auto data = R"({
            "jsonrpc": "2.0",
            "id": 1,
            "params": 123
        })";
        CHECK_THROWS_AS(parseJsonRpcRequest(data), InvalidRequest);
    }
    SUBCASE("Binary without size")
    {
        auto data = "123";
        CHECK_THROWS_AS(parseBinaryJsonRpcRequest(data), ParseError);
    }
    SUBCASE("Binary size bigger than frame size")
    {
        auto data = std::string("\x10\x00\x00\x00", 4) + "{}";
        CHECK_THROWS_AS(parseBinaryJsonRpcRequest(data), ParseError);
    }
}