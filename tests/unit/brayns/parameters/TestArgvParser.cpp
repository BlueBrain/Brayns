/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/parameters/argv/ArgvBuilder.h>
#include <brayns/parameters/argv/ArgvParser.h>
#include <brayns/utils/EnumInfo.h>

namespace
{
struct ArgCustomType
{
};

class ArgInjector
{
public:
    ArgInjector(brayns::ArgvParser &parser):
        _parser(parser)
    {
    }

    void inject(std::vector<const char *> args)
    {
        auto argc = static_cast<int>(args.size());
        auto argv = args.data();
        CHECK_NOTHROW(_parser.parse(argc, argv));
    }

    void injectFailure(std::vector<const char *> args, const char *failMessage)
    {
        auto argc = static_cast<int>(args.size());
        auto argv = args.data();
        CHECK_THROWS_WITH(_parser.parse(argc, argv), doctest::Contains(failMessage));
    }

private:
    brayns::ArgvParser &_parser;
};

enum class ArgEnum
{
    FIRST,
    SECOND
};
}

namespace brayns
{
template<>
struct EnumReflector<ArgEnum>
{
    static EnumMap<ArgEnum> reflect()
    {
        return {{"first", ArgEnum::FIRST}, {"second", ArgEnum::SECOND}};
    }
};
}

TEST_CASE("Parameters Argument parser")
{
    SUBCASE("Invalid type")
    {
        auto properties = std::vector<brayns::ArgvProperty>();

        auto builder = brayns::ArgvBuilder(properties);
        auto variable = ArgCustomType();
        builder.add("param", variable, "");

        auto parser = brayns::ArgvParser(properties);
        auto injector = ArgInjector(parser);
        injector.injectFailure({"executable", "--param", "a_value"}, "Invalid type");
    }
    SUBCASE("Invalid parameter name")
    {
        auto properties = std::vector<brayns::ArgvProperty>();

        auto builder = brayns::ArgvBuilder(properties);
        auto variable = std::string();
        builder.add("param", variable, "");

        auto parser = brayns::ArgvParser(properties);
        auto injector = ArgInjector(parser);
        injector.injectFailure(
            {"executable", "--not_that_param", "a_value"},
            "Unknown keyword argument 'not_that_param'");
    }
    SUBCASE("Unpaired value")
    {
        auto properties = std::vector<brayns::ArgvProperty>();

        auto builder = brayns::ArgvBuilder(properties);
        auto variable = std::string();
        builder.add("param", variable, "");

        auto parser = brayns::ArgvParser(properties);
        auto injector = ArgInjector(parser);
        injector.injectFailure(
            {"executable", "--param", "a_value", "another_value"},
            "Unknown positional argument 'another_value'");
    }
    SUBCASE("Unpaired variable")
    {
        auto properties = std::vector<brayns::ArgvProperty>();

        auto builder = brayns::ArgvBuilder(properties);
        auto variable = std::string();
        builder.add("param", variable, "");

        auto parser = brayns::ArgvParser(properties);
        auto injector = ArgInjector(parser);
        injector.injectFailure({"executable", "--param"}, "No value for keyword argument 'param'");
    }
    SUBCASE("Number parsing")
    {
        auto properties = std::vector<brayns::ArgvProperty>();

        auto builder = brayns::ArgvBuilder(properties);
        auto variable = 0.f;
        builder.add("param", variable, "");

        auto parser = brayns::ArgvParser(properties);
        auto injector = ArgInjector(parser);

        injector.injectFailure({"executable", "--param", "not_a_float"}, "Invalid numeric value");
        injector.injectFailure({"executable", "--param", "true"}, "Invalid numeric value");
        injector.injectFailure({"executable", "--param", ""}, "Invalid numeric value");
        auto overflow = std::to_string(std::numeric_limits<double>::max());
        injector.injectFailure({"executable", "--param", overflow.data()}, "Value above maximum for keyword argument");
        injector.inject({"executable", "--param", "5.2"});
        CHECK(variable == doctest::Approx(5.2));
    }
    SUBCASE("Integer parsing")
    {
        auto properties = std::vector<brayns::ArgvProperty>();

        auto builder = brayns::ArgvBuilder(properties);
        auto variable = 0;
        builder.add("param", variable, "");

        auto parser = brayns::ArgvParser(properties);
        auto injector = ArgInjector(parser);

        injector.injectFailure({"executable", "--param", "not_an_int"}, "Invalid numeric value");
        injector.injectFailure({"executable", "--param", "true"}, "Invalid numeric value");
        injector.injectFailure({"executable", "--param", ""}, "Invalid numeric value");
        auto overflow = std::to_string(std::numeric_limits<unsigned long>::max());
        injector.injectFailure({"executable", "--param", overflow.data()}, "Value above maximum for keyword argument");
        injector.inject({"executable", "--param", "10"});
        CHECK(variable == 10);
    }
    SUBCASE("Bool parsing")
    {
        auto properties = std::vector<brayns::ArgvProperty>();

        auto builder = brayns::ArgvBuilder(properties);
        auto variable = false;
        builder.add("param", variable, "");

        auto parser = brayns::ArgvParser(properties);
        auto injector = ArgInjector(parser);

        injector.injectFailure({"executable", "--param", "not_a_bool"}, "Invalid boolean value");
        injector.injectFailure({"executable", "--param", "5"}, "Invalid boolean value");
        injector.injectFailure({"executable", "--param", ""}, "Invalid boolean value");
        injector.inject({"executable", "--param", "true"});
        CHECK(variable);
    }
    SUBCASE("Vector parsing")
    {
        auto properties = std::vector<brayns::ArgvProperty>();

        auto builder = brayns::ArgvBuilder(properties);
        auto variable = std::vector<int>{0, 0};
        builder.add("param", variable, "").maxItems(2).minItems(2);

        auto parser = brayns::ArgvParser(properties);
        auto injector = ArgInjector(parser);

        injector.injectFailure({"executable", "--param", "not_a_vector"}, "Invalid numeric value");
        injector.injectFailure({"executable", "--param", "5"}, "Not enough items");
        injector.injectFailure({"executable", "--param", "1 2 3"}, "Too many items");
        injector.injectFailure({"executable", "--param", "1 not_a_number 3"}, "Invalid numeric value");
        injector.inject({"executable", "--param", "10 20"});
        CHECK(variable[0] == 10);
        CHECK(variable[1] == 20);
    }
    SUBCASE("Enum parsing")
    {
        auto properties = std::vector<brayns::ArgvProperty>();

        auto builder = brayns::ArgvBuilder(properties);
        auto variable = ArgEnum::FIRST;
        builder.add("param", variable, "");

        auto parser = brayns::ArgvParser(properties);
        auto injector = ArgInjector(parser);

        injector.injectFailure({"executable", "--param", "invalid_enum"}, "Invalid enum value");
        injector.injectFailure({"executable", "--param", "5"}, "Invalid enum value");
        injector.injectFailure({"executable", "--param", ""}, "Invalid enum value");
        injector.inject({"executable", "--param", "second"});
        CHECK(variable == ArgEnum::SECOND);
    };
    SUBCASE("Math type parsing")
    {
        auto properties = std::vector<brayns::ArgvProperty>();

        auto builder = brayns::ArgvBuilder(properties);
        auto variable = brayns::Vector3f(0.f);
        builder.add("param", variable, "");

        auto parser = brayns::ArgvParser(properties);
        auto injector = ArgInjector(parser);

        injector.injectFailure({"executable", "--param", "not_math_type"}, "Invalid numeric value");
        injector.injectFailure({"executable", "--param", "2 not_a_number"}, "Invalid numeric value");
        injector.injectFailure({"executable", "--param", "5"}, "Not enough items");
        injector.injectFailure({"executable", "--param", "1 2 3 10"}, "Too many items");
        injector.inject({"executable", "--param", "10 20 5.2"});
        CHECK(variable.x == doctest::Approx(10.));
        CHECK(variable.y == doctest::Approx(20.));
        CHECK(variable.z == doctest::Approx(5.2));
    }
}
