/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <doctest.h>

#include <brayns/core/api/Api.h>

using namespace brayns::experimental;

TEST_CASE("Api")
{
    SUBCASE("Basic")
    {
        auto offset = 1;

        auto builder = ApiBuilder();

        builder.endpoint("test", [&](int value) { return float(offset + value); }).description("Test");

        auto api = builder.build();

        auto methods = api.getMethods();

        CHECK_EQ(methods.size(), 1);
        CHECK_EQ(methods[0], "test");

        auto schema = api.getSchema("test");

        CHECK_EQ(schema.method, "test");
        CHECK_EQ(schema.description, "Test");
        CHECK_EQ(schema.params, getJsonSchema<int>());
        CHECK_EQ(schema.result, getJsonSchema<float>());
        CHECK_FALSE(schema.binary_params);
        CHECK_FALSE(schema.binary_result);

        auto request = JsonRpcRequest{
            .id = 0,
            .method = "test",
            .params = 2,
        };

        auto response = api.execute(request);

        CHECK_EQ(std::get<int>(response.id), 0);
        CHECK_EQ(response.result.extract<float>(), 3.0f);
        CHECK_EQ(response.binary, "");

        request.method = "invalid";
        CHECK_THROWS_AS(api.execute(request), MethodNotFound);
        request.method = "test";

        request.params = "invalidString";
        CHECK_THROWS_AS(api.execute(request), InvalidParams);
        request.params = 2;

        request.binary = "123";
        CHECK_THROWS_AS(api.execute(request), InvalidParams);
        request.binary = "";

        CHECK_THROWS_AS(api.getSchema("invalidParams"), InvalidParams);
    }
    SUBCASE("No params or no results")
    {
        auto called = false;
        auto buffer = 0;

        auto builder = ApiBuilder();

        builder.endpoint("test1", [] { return 0; });
        builder.endpoint("test2", [&] { called = true; });
        builder.endpoint("test3", [&](int value) { buffer = value; });

        auto api = builder.build();

        auto request = JsonRpcRequest{
            .id = 0,
            .method = "test1",
            .params = {},
        };

        auto response = api.execute(request);
        CHECK_EQ(response.result.extract<int>(), 0);

        request.method = "test2";
        response = api.execute(request);
        CHECK(called);
        CHECK(response.result.isEmpty());

        request.method = "test3";
        request.params = 3;
        response = api.execute(request);
        CHECK(response.result.isEmpty());
        CHECK_EQ(buffer, 3);
    }
}
