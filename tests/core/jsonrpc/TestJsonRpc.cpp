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

using namespace brayns;

std::string mockJson()
{
    return R"({
        "jsonrpc": "2.0",
        "id": 1,
        "method": "test",
        "params": 123
    })";
}

TEST_CASE("Text")
{
    auto json = mockJson();

    auto request = parseJsonRpcRequest(json);

    CHECK(request.id);
    CHECK_EQ(std::get<int>(*request.id), 1);
    CHECK_EQ(request.method, "test");
    CHECK_EQ(request.params.json, 123);
    CHECK(request.params.binary.empty());
}

TEST_CASE("Binary")
{
    auto json = mockJson();

    auto data = std::vector<char>();

    auto jsonSize = static_cast<std::uint32_t>(json.size());
    composeBytesTo(jsonSize, std::endian::little, data);

    data.insert(data.end(), json.begin(), json.end());

    auto binary = std::string("binary");
    data.insert(data.end(), binary.begin(), binary.end());

    auto request = parseJsonRpcRequest(data);

    CHECK(request.id);
    CHECK_EQ(std::get<int>(*request.id), 1);
    CHECK_EQ(request.method, "test");
    CHECK_EQ(request.params.json, 123);
    CHECK_EQ(request.params.binary, std::vector<char>{binary.begin(), binary.end()});
}

TEST_CASE("Invalid JSON")
{
    auto data = "{\"test";
    CHECK_THROWS_AS(parseJsonRpcRequest(data), ParseError);
}

TEST_CASE("Invalid schema")
{
    auto data = R"({
        "id": 1,
        "method": "test",
        "params": 123
    })";

    CHECK_THROWS_AS(parseJsonRpcRequest(data), InvalidRequest);
}

TEST_CASE("Invalid JSON-RPC version")
{
    auto data = R"({
        "jsonrpc": "1.0",
        "id": 1,
        "method": "test",
        "params": 123
    })";

    CHECK_THROWS_AS(parseJsonRpcRequest(data), InvalidRequest);
}

TEST_CASE("No methods")
{
    auto data = R"({
        "jsonrpc": "2.0",
        "id": 1,
        "params": 123
    })";

    CHECK_THROWS_AS(parseJsonRpcRequest(data), InvalidRequest);
}

TEST_CASE("Binary without size")
{
    auto data = std::vector<char>{'1', '2', '3'};
    CHECK_THROWS_AS(parseJsonRpcRequest(data), ParseError);
}

TEST_CASE("Binary size bigger than frame size")
{
    auto text = std::string("\x10\x00\x00\x00", 4) + "{}";
    auto data = std::vector<char>(text.begin(), text.end());
    CHECK_THROWS_AS(parseJsonRpcRequest(data), ParseError);
}
