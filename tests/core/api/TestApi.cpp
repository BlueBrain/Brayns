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

using namespace brayns;

TEST_CASE("Basic")
{
    auto offset = 1;

    auto endpoints = EndpointRegistry();
    auto builder = ApiBuilder(endpoints);

    builder.endpoint("test", [&](int value) { return float(offset + value); }).description("Test");

    auto methods = endpoints.getMethods();

    CHECK_EQ(methods.size(), 1);
    CHECK_EQ(methods[0], "test");

    const auto &schema = endpoints.getSchema("test");

    CHECK_EQ(schema.method, "test");
    CHECK_EQ(schema.description, "Test");
    CHECK_EQ(schema.params, getJsonSchema<int>());
    CHECK_EQ(schema.result, getJsonSchema<float>());

    auto params = Payload{2};

    auto result = endpoints.start("test", params).run();

    CHECK_EQ(result.json.extract<float>(), 3.0f);
    CHECK(result.binary.empty());
}

TEST_CASE("With binary")
{
    auto endpoints = EndpointRegistry();
    auto builder = ApiBuilder(endpoints);

    auto value = 0;
    auto buffer = std::vector<char>();

    builder.endpoint("test1", [](int) { return NullJson(); });
    builder.endpoint(
        "test2",
        [&](Params<int> params)
        {
            value = params.value;
            buffer = params.binary;
            return Result<int>{2, {1, 2, 3, 4}};
        });

    auto params = Payload{1, {1, 2, 3}};

    auto result = endpoints.start("test2", params).run();

    CHECK_EQ(value, 1);
    CHECK_EQ(buffer, std::vector<char>{1, 2, 3});

    CHECK_EQ(result.json, 2);
    CHECK_EQ(result.binary, std::vector<char>{1, 2, 3, 4});
}

TEST_CASE("No params or result")
{
    auto endpoints = EndpointRegistry();
    auto builder = ApiBuilder(endpoints);

    builder.endpoint("test1", [] { return 0; });
    builder.endpoint("test2", [](int) {});
    builder.endpoint("test3", [] {});

    CHECK_EQ(endpoints.getSchema("test1").params, getJsonSchema<NullJson>());
    CHECK_EQ(endpoints.getSchema("test1").result, getJsonSchema<int>());
    CHECK_EQ(endpoints.getSchema("test2").params, getJsonSchema<int>());
    CHECK_EQ(endpoints.getSchema("test2").result, getJsonSchema<NullJson>());
    CHECK_EQ(endpoints.getSchema("test3").params, getJsonSchema<NullJson>());
    CHECK_EQ(endpoints.getSchema("test3").result, getJsonSchema<NullJson>());

    CHECK_EQ(endpoints.start("test1", Payload()).run().json, serializeToJson(0));
    CHECK_EQ(endpoints.start("test2", Payload(0)).run().json, serializeToJson(NullJson()));
    CHECK_EQ(endpoints.start("test3", Payload()).run().json, serializeToJson(NullJson()));
}

struct NonCopyable
{
    NonCopyable() = default;
    ~NonCopyable() = default;

    NonCopyable(const NonCopyable &) = delete;
    NonCopyable(NonCopyable &&) = default;

    NonCopyable &operator=(const NonCopyable &) = delete;
    NonCopyable &operator=(NonCopyable &&) = default;
};

namespace brayns
{
template<>
struct JsonObjectReflector<NonCopyable>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<NonCopyable>();
        return builder.build();
    }
};
}

TEST_CASE("Copy")
{
    auto endpoints = EndpointRegistry();
    auto builder = ApiBuilder(endpoints);

    builder.endpoint("test", [](NonCopyable) { return NonCopyable(); });

    auto params = Payload{createJsonObject()};

    auto result = endpoints.start("test", params).run();

    CHECK(getObject(result.json).size() == 0);
    CHECK(result.binary.empty());
}

TEST_CASE("Duplication")
{
    auto endpoints = EndpointRegistry();
    auto builder = ApiBuilder(endpoints);

    auto handler = [](NullJson) { return NullJson(); };

    builder.endpoint("test", handler);
    CHECK_THROWS_AS(builder.endpoint("test", handler), std::invalid_argument);
}
