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

#include <brayns/core/api/ApiBuilder.h>

using namespace brayns::experimental;

TEST_CASE("Basic")
{
    auto offset = 1;

    auto builder = ApiBuilder();

    builder.endpoint("test", [&](int value) { return float(offset + value); }).description("Test");

    auto endpoints = builder.build();

    auto methods = endpoints.getMethods();

    CHECK_EQ(methods.size(), 1);
    CHECK_EQ(methods[0], "test");

    const auto *endpoint = endpoints.find("test");

    CHECK(endpoint != nullptr);

    const auto &schema = endpoint->schema;

    CHECK_EQ(schema.method, "test");
    CHECK_EQ(schema.description, "Test");
    CHECK_EQ(schema.params, getJsonSchema<int>());
    CHECK_EQ(schema.result, getJsonSchema<float>());

    auto params = RawParams{2};

    auto result = endpoint->run(params);

    CHECK_EQ(result.json.extract<float>(), 3.0f);
    CHECK_EQ(result.binary, "");
}

TEST_CASE("No params or no results")
{
    auto called = false;
    auto buffer = 0;

    auto builder = ApiBuilder();

    builder.endpoint("test1", [] { return 0; });
    builder.endpoint("test2", [&] { called = true; });
    builder.endpoint("test3", [&](int value) { buffer = value; });

    auto endpoints = builder.build();

    auto params = RawParams();

    const auto *endpoint = endpoints.find("test1");
    CHECK(endpoint != nullptr);
    auto result = endpoint->run(params);
    CHECK_EQ(result.json.extract<int>(), 0);

    endpoint = endpoints.find("test2");
    CHECK(endpoint != nullptr);
    result = endpoint->run(params);
    CHECK(called);
    CHECK(result.json.isEmpty());

    endpoint = endpoints.find("test3");
    CHECK(endpoint != nullptr);
    params.json = 3;
    result = endpoint->run(params);
    CHECK(result.json.isEmpty());
    CHECK_EQ(buffer, 3);
}

TEST_CASE("With binary")
{
    auto builder = ApiBuilder();

    auto value = 0;
    auto buffer = std::string();

    builder.endpoint("test1", [](int) {});
    builder.endpoint(
        "test2",
        [&](Params<int> params)
        {
            value = params.value;
            buffer = params.binary;
            return Result<int>{2, "1234"};
        });

    auto endpoints = builder.build();

    auto normal = endpoints.find("test1");
    CHECK(normal != nullptr);

    auto params = RawParams{1, "123"};

    CHECK_THROWS_AS(normal->run(params), InvalidParams);

    auto binary = endpoints.find("test2");
    CHECK(binary != nullptr);

    auto result = binary->run(params);

    CHECK_EQ(value, 1);
    CHECK_EQ(buffer, "123");

    CHECK_EQ(result.json, 2);
    CHECK_EQ(result.binary, "1234");
}
