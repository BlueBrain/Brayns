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

#include <brayns/core/json/Json.h>

struct Internal
{
    int test1 = 0;
    int test2 = 0;
    int test3 = 0;

    int get() const
    {
        return 4;
    }

    void set(int value)
    {
        test2 = 2 * value;
    }
};

struct Test
{
    int normal = 0;
    int readOnly = 0;
    int writeOnly = 0;
    int optional = 0;
    brayns::Vector2f defaulted{0};
    double number = 0;
    std::vector<int> array;
    Internal internal;
};

namespace brayns
{
template<>
struct JsonAdapter<Internal> : ObjectAdapter<Internal>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Internal");
        builder.getset(
            "getset",
            [](auto &object) { return object.test1; },
            [](auto &object, auto value) { object.test1 = value; });
        builder.get("get", [](auto &object) { return object.test2; });
        builder.set<int>("set", [](auto &object, auto value) { object.test3 = value; });
        builder
            .getset(
                "methods",
                [](auto &object) { return object.get(); },
                [](auto &object, auto value) { object.set(value); })
            .defaultValue(1);
        return builder.build();
    }
};

template<>
struct JsonAdapter<Test> : ObjectAdapter<Test>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("Test");
        builder
            .getset(
                "normal",
                [](auto &object) { return object.normal; },
                [](auto &object, auto value) { object.normal = value; })
            .description("Test normal");
        builder
            .getset(
                "readOnly",
                [](auto &object) { return object.readOnly; },
                [](auto &object, auto value) { object.readOnly = value; })
            .description("Test readOnly")
            .readOnly(true);
        builder
            .getset(
                "writeOnly",
                [](auto &object) { return object.writeOnly; },
                [](auto &object, auto value) { object.writeOnly = value; })
            .description("Test writeOnly")
            .writeOnly(true);
        builder
            .getset(
                "optional",
                [](auto &object) { return object.optional; },
                [](auto &object, auto value) { object.optional = value; })
            .description("Test optional")
            .required(false);
        builder
            .getset(
                "defaulted",
                [](auto &object) { return object.defaulted; },
                [](auto &object, auto value) { object.defaulted = value; })
            .description("Test defaulted")
            .defaultValue(brayns::Vector2f(1));
        builder
            .getset(
                "number",
                [](auto &object) { return object.number; },
                [](auto &object, auto value) { object.number = value; })
            .description("Test number")
            .minimum(1)
            .maximum(3);
        builder
            .getset(
                "array",
                [](auto &object) -> auto & { return object.array; },
                [](auto &object, auto value) { object.array = std::move(value); })
            .description("Test array")
            .minItems(2)
            .maxItems(4);
        builder
            .getset(
                "internal",
                [](auto &object) -> auto & { return object.internal; },
                [](auto &object, auto value) { object.internal = std::move(value); })
            .description("Test internal");
        return builder.build();
    }
};
} // namespace brayns

TEST_CASE("Schema")
{
    SUBCASE("Title and type")
    {
        auto schema = brayns::Json::getSchema<Test>();
        CHECK_EQ(schema.title, "Test");
        CHECK_EQ(schema.type, brayns::JsonType::Object);
    }
    SUBCASE("Read and write only")
    {
        auto schema = brayns::Json::getSchema<Test>();
        auto &normal = schema.properties.at("normal");
        CHECK_FALSE(normal.readOnly);
        CHECK_FALSE(normal.writeOnly);
        auto &readOnly = schema.properties.at("readOnly");
        CHECK(readOnly.readOnly);
        CHECK_FALSE(readOnly.writeOnly);
        auto &writeOnly = schema.properties.at("writeOnly");
        CHECK_FALSE(writeOnly.readOnly);
        CHECK(writeOnly.writeOnly);
    }
    SUBCASE("Required and default")
    {
        auto schema = brayns::Json::getSchema<Test>();
        CHECK_FALSE(schema.properties.at("optional").required);
        CHECK_FALSE(schema.properties.at("defaulted").required);
        CHECK(schema.properties.at("normal").required);
        auto &defaultValue = schema.properties.at("defaulted").defaultValue;
        CHECK_EQ(brayns::Json::deserialize<brayns::Vector2f>(defaultValue), brayns::Vector2f(1));
    }
    SUBCASE("Min and max")
    {
        auto schema = brayns::Json::getSchema<Test>();
        auto &number = schema.properties.at("number");
        CHECK_EQ(number.type, brayns::JsonType::Number);
        CHECK_EQ(number.minimum, 1);
        CHECK_EQ(number.maximum, 3);
    }
    SUBCASE("Min and max items")
    {
        auto schema = brayns::Json::getSchema<Test>();
        auto &array = schema.properties.at("array");
        CHECK_EQ(array.type, brayns::JsonType::Array);
        CHECK_EQ(array.items.at(0).type, brayns::JsonType::Integer);
        CHECK_EQ(array.minItems, 2);
        CHECK_EQ(array.maxItems, 4);
    }
    SUBCASE("Internal")
    {
        auto schema = brayns::Json::getSchema<Test>();
        auto &internal = schema.properties.at("internal");
        CHECK_EQ(internal.title, "Internal");
        CHECK_EQ(internal.type, brayns::JsonType::Object);
        CHECK_EQ(internal.properties.size(), 4);
        auto &property = internal.properties.at("get");
        CHECK_EQ(property.type, brayns::JsonType::Integer);
        CHECK(property.required);
    }
}

TEST_CASE("Parsing")
{
    SUBCASE("Parse")
    {
        auto json = R"({"getset": 1, "set": 2, "methods": 3})";
        auto internal = brayns::Json::parse<Internal>(json);
        CHECK_EQ(internal.test1, 1);
        CHECK_EQ(internal.test2, 6);
        CHECK_EQ(internal.test3, 2);
    }
    SUBCASE("Default")
    {
        auto json = R"({"getset": 1, "set": 3})";
        auto internal = brayns::Json::parse<Internal>(json);
        CHECK_EQ(internal.test1, 1);
        CHECK_EQ(internal.test2, 2);
        CHECK_EQ(internal.test3, 3);
    }
    SUBCASE("Stringify")
    {
        auto ref = R"({"get":2,"getset":1,"methods":4})";
        auto test = Internal{1, 2, 3};
        auto json = brayns::Json::stringify(test);
        CHECK_EQ(json, ref);
    }
}
