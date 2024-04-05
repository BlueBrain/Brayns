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

#include <doctest/doctest.h>

#include <brayns/core/network/entrypoint/EntrypointBuilder.h>
#include <brayns/core/network/entrypoint/EntrypointFinder.h>

#include "MockEntrypoint.h"
#include "MockInterface.h"
#include "MockWebSocket.h"

class MockMessageFactory
{
public:
    static brayns::RequestMessage createMessage()
    {
        auto message = brayns::RequestMessage();
        message.jsonrpc = "2.0";
        message.id = brayns::RequestId(0);
        message.method = "test";
        message.params = 123;
        return message;
    }
};

TEST_CASE("EntrypointBuilder")
{
    auto interface = MockInterface();
    auto plugin = std::string("Plugin");
    auto builder = brayns::EntrypointBuilder(plugin, interface);
    builder.add<MockEntrypoint>();
    auto &entrypoints = interface.getEntrypoints();
    CHECK_EQ(entrypoints.size(), 1);
    auto &entrypoint = entrypoints.front();
    CHECK_EQ(entrypoint.getPlugin(), plugin);
}

TEST_CASE("EntrypointFinder")
{
    auto entrypoints = brayns::EntrypointRegistry();
    entrypoints.add({"plugin", std::make_unique<MockEntrypoint>()});
    auto socket = std::make_shared<MockWebSocket>();
    auto client = brayns::ClientRef(socket);
    auto find = [&](const auto &request) -> decltype(auto)
    { return brayns::EntrypointFinder::find(request, entrypoints); };

    SUBCASE("Valid request")
    {
        auto message = MockMessageFactory::createMessage();
        auto request = brayns::JsonRpcRequest(client, message);
        auto &entrypoint = find(request);
        CHECK_EQ(entrypoint.getMethod(), message.method);
    }
    SUBCASE("Invalid method")
    {
        auto message = MockMessageFactory::createMessage();
        message.method = "invalid";
        auto request = brayns::JsonRpcRequest(client, message);
        CHECK_THROWS_AS(find(request), brayns::MethodNotFoundException);
    }
    SUBCASE("Invalid params")
    {
        auto message = MockMessageFactory::createMessage();
        message.params = "invalid";
        auto request = brayns::JsonRpcRequest(client, message);
        CHECK_THROWS_AS(find(request), brayns::InvalidParamsException);
    }
}

TEST_CASE("EntrypointRegistry")
{
    auto entrypoints = brayns::EntrypointRegistry();
    auto add = [&](const auto &name) { entrypoints.add({"plugin", std::make_unique<MockEntrypoint>(name)}); };

    SUBCASE("Valid usage")
    {
        add("test");
        CHECK(entrypoints.find("test"));
    }
    SUBCASE("Invalid usage")
    {
        add("test");
        CHECK_THROWS_AS(add("test"), std::invalid_argument);
        CHECK_THROWS_AS(add(""), std::invalid_argument);
    }
}
