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

#include <brayns/core/cli/CommandLine.h>

#include <doctest.h>

using namespace brayns::experimental;

namespace brayns::experimental
{
struct SomeSettings
{
    bool someBool = false;
    int someInt = 0;
    float someFloat = 0.0F;
    std::string someString = {};
};

template<>
struct ArgvSettingsReflector<SomeSettings>
{
    static auto reflect()
    {
        auto builder = ArgvBuilder<SomeSettings>();
        builder.description("This is a test");
        builder.option("some-bool", [](auto &settings) { return &settings.someBool; })
            .description("Test 1")
            .defaultValue(true);
        builder.option("some-int", [](auto &settings) { return &settings.someInt; })
            .description("Test 2")
            .defaultValue(1);
        builder.option("some-float", [](auto &settings) { return &settings.someFloat; })
            .description("Test 3")
            .defaultValue(2.0F);
        builder.option("some-string", [](auto &settings) { return &settings.someString; })
            .description("Test 4")
            .defaultValue("123");
        return builder.build();
    }
};
}

TEST_CASE("ParsingArgv")
{
    int argc = 5;
    const char *argv[] = {
        "program",
        "--test1",
        "value1",
        "--test2",
        "value2",
    };

    auto test = parseArgv(argc, argv);
    auto ref = ArgvMap{{"test1", "value1"}, {"test2", "value2"}};

    CHECK_EQ(test, ref);

    int argc2 = 2;
    const char *argv2[] = {"program", "valueWithoutKey"};

    CHECK_THROWS_AS(parseArgv(argc2, argv2), std::invalid_argument);
}

TEST_CASE("Parsing")
{
    auto argv = ArgvMap{
        {"some-bool", "true"},
        {"some-int", "2"},
        {"some-float", "3"},
        {"some-string", "1234"},
    };

    auto settings = parseArgvAs<SomeSettings>(argv);

    CHECK(settings.someBool);
    CHECK_EQ(settings.someInt, 2);
    CHECK_EQ(settings.someFloat, 3.0F);
    CHECK_EQ(settings.someString, "1234");
}

TEST_CASE("Default")
{
    auto argv = ArgvMap{};

    auto settings = parseArgvAs<SomeSettings>(argv);

    CHECK(settings.someBool);
    CHECK_EQ(settings.someInt, 1);
    CHECK_EQ(settings.someFloat, 2.0F);
    CHECK_EQ(settings.someString, "123");
}

TEST_CASE("Help")
{
    auto help = getArgvHelp<SomeSettings>();

    auto ref =
        "This is a test\n\n"
        "Options:\n"
        "    --some-bool boolean: Test 1 (default true)\n"
        "    --some-float number: Test 3 (default 2)\n"
        "    --some-int integer: Test 2 (default 1)\n"
        "    --some-string string: Test 4 (default 123)\n";

    CHECK_EQ(help, ref);
}
