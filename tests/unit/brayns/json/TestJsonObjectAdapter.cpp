/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/json/Json.h>

struct Test
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

namespace brayns
{
template<>
struct JsonAdapter<Test> : ObjectAdapter<Test>
{
    static void reflect()
    {
        title("Test");
        getset(
            "getset",
            [](auto &object) { return object.test1; },
            [](auto &object, auto value) { object.test1 = value; })
            .description("Test getset");
        get("get", [](auto &object) { return object.test2; }).description("Test get").required(false);
        set<int>("set", [](auto &object, auto value) { object.test3 = value; }).description("Test set");
        getset(
            "methods",
            [](auto &object) { return object.get(); },
            [](auto &object, auto value) { object.set(value); })
            .description("Test methods");
    }
};
} // namespace brayns

TEST_CASE("JsonObjectAdapter")
{
    SUBCASE("Schema")
    {
        auto schema = brayns::Json::getSchema<Test>();
        CHECK_EQ(schema.title, "Test");
        CHECK_EQ(schema.type, brayns::JsonType::Object);
        CHECK_EQ(schema.properties.at("getset").type, brayns::JsonType::Integer);
        CHECK_EQ(schema.properties.at("get").type, brayns::JsonType::Integer);
        CHECK_EQ(schema.properties.at("set").type, brayns::JsonType::Integer);
        CHECK_EQ(schema.properties.at("methods").type, brayns::JsonType::Integer);
        CHECK_EQ(schema.properties.at("getset").description, "Test getset");
        CHECK_FALSE(schema.properties.at("get").required);
        CHECK(schema.properties.at("get").readOnly);
        CHECK_FALSE(schema.properties.at("get").writeOnly);
        CHECK(schema.properties.at("set").writeOnly);
        CHECK_FALSE(schema.properties.at("set").readOnly);
    }
    SUBCASE("Serialize")
    {
        auto test = Test{1, 2, 3};
        auto json = brayns::Json::serialize(test);
        auto object = brayns::JsonExtractor::extractObject(json);
        CHECK_EQ(object.size(), 3);
        CHECK_EQ(object.get("getset").convert<int>(), 1);
        CHECK_EQ(object.get("get").convert<int>(), 2);
        CHECK_EQ(object.get("methods").convert<int>(), 4);
    }
    SUBCASE("Deserialize")
    {
        auto json = R"({"getset": 1, "set": 2, "methods": 3})";
        auto test = brayns::Json::parse<Test>(json);
        CHECK_EQ(test.test1, 1);
        CHECK_EQ(test.test2, 6);
        CHECK_EQ(test.test3, 2);
    }
}
