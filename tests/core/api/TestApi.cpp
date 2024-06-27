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
    CHECK_FALSE(schema.async);

    auto params = RawParams{2};

    auto result = api.execute("test", params);

    CHECK_EQ(result.json.extract<float>(), 3.0f);
    CHECK_EQ(result.binary, "");
}

TEST_CASE("With binary")
{
    auto builder = ApiBuilder();

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

    auto params = RawParams{1, "123"};

    CHECK_THROWS_AS(api.execute("test1", params), InvalidParams);

    auto result = api.execute("test2", params);

    CHECK_EQ(value, 1);
    CHECK_EQ(buffer, "123");

    CHECK_EQ(result.json, 2);
    CHECK_EQ(result.binary, "1234");
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

    builder.endpoint("test", [](NonCopyable) { return NonCopyable(); });

    auto api = builder.build();

    auto params = RawParams{createJsonObject()};

    auto result = api.execute("test", params);

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

TEST_CASE("Task")
{
    auto builder = ApiBuilder();

    auto offset = 1;

    auto worker = [&](Progress progress, int value)
    {
        progress.nextOperation("1");
        progress.update(0.5F);
        return value + offset;
    };

    builder.task("test", [=](int value) { return startTask(worker, value); });

    auto api = builder.build();

    auto params = RawParams{2};

    auto [json, binary] = api.execute("test", params);

    CHECK(binary.empty());

    auto object = getObject(json);
    auto taskId = object.get("task_id").extract<TaskId>();

    CHECK_EQ(taskId, 0);

    auto tasks = api.getTasks();

    CHECK_EQ(tasks.size(), 1);

    while (true)
    {
        auto progress = api.getTaskProgress(taskId);

        if (progress.currentOperationProgress != 0.0F)
        {
            CHECK_EQ(progress.currentOperation, "1");
            CHECK_EQ(progress.currentOperationProgress, 0.5F);
            break;
        }
    }

    auto result = api.waitForTaskResult(taskId);

    CHECK_EQ(result.json.extract<int>(), 3);
    CHECK(result.binary.empty());

    CHECK_THROWS_AS(api.cancelTask(taskId), InvalidParams);
    CHECK_THROWS_AS(api.waitForTaskResult(taskId), InvalidParams);
    CHECK_THROWS_AS(api.getTaskProgress(taskId), InvalidParams);

    CHECK(api.getTasks().empty());
}
