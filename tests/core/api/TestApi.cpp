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

    auto builder = ApiBuilder();
    auto monitor = std::make_shared<TaskMonitor>(1);
    auto progress = Progress(monitor);

    builder.endpoint("test", [&](int value) { return float(offset + value); }).description("Test");

    auto api = builder.build();

    auto methods = api.getMethods();

    CHECK_EQ(methods.size(), 1);
    CHECK_EQ(methods[0], "test");

    const auto &schema = api.getSchema("test");

    CHECK_EQ(schema.method, "test");
    CHECK_EQ(schema.description, "Test");
    CHECK_EQ(schema.params, getJsonSchema<int>());
    CHECK_EQ(schema.result, getJsonSchema<float>());

    auto params = Payload{2};

    auto result = api.execute("test", params, progress);

    CHECK_EQ(result.json.extract<float>(), 3.0f);
    CHECK_EQ(result.binary, "");
}

TEST_CASE("With binary")
{
    auto builder = ApiBuilder();
    auto monitor = std::make_shared<TaskMonitor>(1);
    auto progress = Progress(monitor);

    auto value = 0;
    auto buffer = std::string();

    builder.endpoint("test1", [](int) { return NullJson(); });
    builder.endpoint(
        "test2",
        [&](Params<int> params)
        {
            value = params.value;
            buffer = params.binary;
            return Result<int>{2, "1234"};
        });

    auto api = builder.build();

    auto params = Payload{1, "123"};

    CHECK_THROWS_AS(api.execute("test1", params, progress), InvalidParams);

    auto result = api.execute("test2", params, progress);

    CHECK_EQ(value, 1);
    CHECK_EQ(buffer, "123");

    CHECK_EQ(result.json, 2);
    CHECK_EQ(result.binary, "1234");
}

TEST_CASE("No params or result")
{
    auto builder = ApiBuilder();
    auto monitor = std::make_shared<TaskMonitor>(1);
    auto progress = Progress(monitor);

    builder.endpoint("test1", [] { return 0; });
    builder.endpoint("test2", [](int) {});
    builder.endpoint("test3", [] {});

    auto api = builder.build();

    CHECK_EQ(api.getSchema("test1").params, getJsonSchema<NullJson>());
    CHECK_EQ(api.getSchema("test1").result, getJsonSchema<int>());
    CHECK_EQ(api.getSchema("test2").params, getJsonSchema<int>());
    CHECK_EQ(api.getSchema("test2").result, getJsonSchema<NullJson>());
    CHECK_EQ(api.getSchema("test3").params, getJsonSchema<NullJson>());
    CHECK_EQ(api.getSchema("test3").result, getJsonSchema<NullJson>());

    CHECK_EQ(api.execute("test1", Payload(), progress).json, serializeToJson(0));
    CHECK_EQ(api.execute("test2", Payload(0), progress).json, serializeToJson(NullJson()));
    CHECK_EQ(api.execute("test3", Payload(), progress).json, serializeToJson(NullJson()));
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
    auto builder = ApiBuilder();
    auto monitor = std::make_shared<TaskMonitor>(1);
    auto progress = Progress(monitor);

    builder.endpoint("test", [](NonCopyable) { return NonCopyable(); });

    auto api = builder.build();

    auto params = Payload{createJsonObject()};

    auto result = api.execute("test", params, progress);

    CHECK(getObject(result.json).size() == 0);
    CHECK(result.binary.empty());
}

TEST_CASE("Duplication")
{
    auto builder = ApiBuilder();

    auto handler = [](NullJson) { return NullJson(); };

    builder.endpoint("test", handler);
    CHECK_THROWS_AS(builder.endpoint("test", handler), std::invalid_argument);
}
