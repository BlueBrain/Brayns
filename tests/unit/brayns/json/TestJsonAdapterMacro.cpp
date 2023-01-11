/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/json/JsonAdapterMacro.h>

struct Test
{
    int test1 = 0;
    int test2 = 0;
    int test3 = 0;

    int getTest() const
    {
        return 4;
    }

    void setTest(int value)
    {
        test3 = value;
    }
};

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(Test)
BRAYNS_JSON_ADAPTER_NAMED_ENTRY("abc", test1, "Test")
BRAYNS_JSON_ADAPTER_ENTRY(test2, "Test")
BRAYNS_JSON_ADAPTER_GETSET("test", getTest, setTest, "Test getset")
BRAYNS_JSON_ADAPTER_END()
}

TEST_CASE("JsonAdapterMacro")
{
    SUBCASE("Schema")
    {
        auto schema = brayns::Json::getSchema<Test>();
        CHECK_EQ(schema.type, brayns::JsonType::Object);
        CHECK_EQ(schema.properties.at("abc").type, brayns::JsonType::Integer);
        CHECK_EQ(schema.properties.at("test2").type, brayns::JsonType::Integer);
        CHECK_EQ(schema.properties.at("test").type, brayns::JsonType::Integer);
        CHECK_EQ(schema.required.size(), 2);
    }
    SUBCASE("Serialize")
    {
        auto test = Test{1, 2, 3};
        auto json = brayns::Json::serialize(test);
        auto object = brayns::JsonExtractor::extractObject(json);
        CHECK(object);
        CHECK_EQ(object->get("abc"), 1);
        CHECK_EQ(object->get("test2"), 2);
        CHECK_EQ(object->get("test"), 4);
    }
    SUBCASE("Deserialize")
    {
        auto json = R"({"abc": 1, "test2": 2, "test": 3})";
        auto test = brayns::Json::parse<Test>(json);
        CHECK_EQ(test.test1, 1);
        CHECK_EQ(test.test2, 2);
        CHECK_EQ(test.test3, 3);
    }
}
