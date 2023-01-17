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

#include <brayns/json/JsonObjectMacro.h>

BRAYNS_JSON_OBJECT_BEGIN(Internal)
BRAYNS_JSON_OBJECT_ENTRY(std::string, test, "Test")
BRAYNS_JSON_OBJECT_END()

BRAYNS_JSON_OBJECT_BEGIN(Test)
BRAYNS_JSON_OBJECT_ENTRY(int, normal, "Test normal")
BRAYNS_JSON_OBJECT_ENTRY(int, readOnly, "Test read only", brayns::ReadOnly())
BRAYNS_JSON_OBJECT_ENTRY(int, writeOnly, "Test write only", brayns::WriteOnly())
BRAYNS_JSON_OBJECT_ENTRY(int, optional, "Test optional", brayns::Required(false))
BRAYNS_JSON_OBJECT_ENTRY(int, defaulted, "Test optional", brayns::Default(2))
BRAYNS_JSON_OBJECT_ENTRY(double, number, "Test number", brayns::Minimum(1), brayns::Maximum(3))
BRAYNS_JSON_OBJECT_ENTRY(std::vector<int>, array, "Test array", brayns::MinItems(2), brayns::MaxItems(4))
BRAYNS_JSON_OBJECT_END()

TEST_CASE("JsonObjectMacro")
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
        CHECK_FALSE(brayns::JsonSchemaHelper::isRequired(schema, "optional"));
        CHECK_FALSE(brayns::JsonSchemaHelper::isRequired(schema, "defaulted"));
        CHECK(brayns::JsonSchemaHelper::isRequired(schema, "normal"));
        CHECK_EQ(schema.properties.at("defaulted").defaultValue, 2);
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
    SUBCASE("Serialize")
    {
        auto ref = R"({"test":"abcd"})";
        auto test = Internal();
        test.test = "abcd";
        auto json = brayns::Json::stringify(test);
        CHECK_EQ(json, ref);
    }
    SUBCASE("Deserialize")
    {
        auto json = R"({"test": "abcd"})";
        auto test = brayns::Json::parse<Internal>(json);
        CHECK_EQ(test.test, "abcd");
    }
}
