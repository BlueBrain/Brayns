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

TEST_CASE("JsonValidator")
{
    SUBCASE("Wildcard")
    {
        auto schema = brayns::JsonSchema();
        auto json = brayns::Json::parse(R"({"test": 10})");

        auto errors = brayns::Json::validate(json, schema);
        CHECK(errors.empty());

        json = 1;
        errors = brayns::Json::validate(json, schema);
        CHECK(errors.empty());
    }
    SUBCASE("One of")
    {
        auto schema = brayns::JsonSchema();
        schema.oneOf = {brayns::Json::getSchema<double>(), brayns::Json::getSchema<std::string>()};

        auto errors = brayns::Json::validate(1.0f, schema);
        CHECK(errors.empty());

        errors = brayns::Json::validate("Test", schema);
        CHECK(errors.empty());

        errors = brayns::Json::validate(true, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0].message, "invalid oneOf, no schemas match input");
    }
    SUBCASE("Invalid type")
    {
        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::String;

        auto errors = brayns::Json::validate(1, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0].message, "invalid type, expected string got integer");

        schema.type = brayns::JsonType::Number;
        errors = brayns::Json::validate(1, schema);
        CHECK(errors.empty());
    }
    SUBCASE("Limits")
    {
        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::Integer;
        schema.minimum = -1;
        schema.maximum = 3;

        auto errors = brayns::Json::validate(1, schema);
        CHECK(errors.empty());

        errors = brayns::Json::validate(-1, schema);
        CHECK(errors.empty());

        errors = brayns::Json::validate(3, schema);
        CHECK(errors.empty());

        errors = brayns::Json::validate(-2, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0].message, "value below minimum -2 < -1");

        errors = brayns::Json::validate(4, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0].message, "value above maximum 4 > 3");

        schema.minimum = std::nullopt;
        schema.maximum = std::nullopt;
        errors = brayns::Json::validate(-8, schema);
        CHECK(errors.empty());
        errors = brayns::Json::validate(125, schema);
        CHECK(errors.empty());
    }
    SUBCASE("Enums")
    {
        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::String;
        schema.enums = {"test1", "test2"};

        auto errors = brayns::Json::validate("test1", schema);
        CHECK(errors.empty());

        errors = brayns::Json::validate("test2", schema);
        CHECK(errors.empty());

        errors = brayns::Json::validate("Test2", schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0].message, "invalid enum 'Test2' not in ['test1', 'test2']");
    }
    SUBCASE("Property type")
    {
        auto internal = brayns::JsonSchema();
        internal.type = brayns::JsonType::Object;
        internal.properties = {{"integer", brayns::Json::getSchema<int>()}};

        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::Object;
        schema.properties = {{"internal", internal}};

        auto json = brayns::Json::parse(R"({"internal": 1})");
        auto errors = brayns::Json::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0].message, "invalid type, expected object got integer");

        json = brayns::Json::parse(R"({"internal": {"integer": true}})");
        errors = brayns::Json::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0].message, "invalid type, expected integer got boolean");

        json = brayns::Json::parse(R"({"internal": {"integer": 1}})");
        errors = brayns::Json::validate(json, schema);
        CHECK(errors.empty());
    }
    SUBCASE("Missing property")
    {
        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::Object;
        schema.properties = {
            {"integer", brayns::Json::getSchema<int>()},
            {"string", brayns::Json::getSchema<std::string>()}};
        schema.properties.at("integer").required = true;

        auto json = brayns::Json::parse(R"({"integer": 1, "string": "test"})");
        auto errors = brayns::Json::validate(json, schema);
        CHECK(errors.empty());

        json = brayns::Json::parse(R"({"string": "test"})");
        errors = brayns::Json::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0].message, "missing required property 'integer'");

        json = brayns::Json::parse(R"({"integer": 1})");
        errors = brayns::Json::validate(json, schema);
        CHECK(errors.empty());
    }
    SUBCASE("Unknown properties")
    {
        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::Object;
        auto readOnly = brayns::Json::getSchema<int>();
        readOnly.required = true;
        readOnly.readOnly = true;
        schema.properties["readOnly"] = std::move(readOnly);

        auto json = brayns::Json::parse(R"({"something": 1})");
        auto errors = brayns::Json::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0].message, "unknown property 'something'");

        json = brayns::Json::parse(R"({"readOnly": 1})");
        errors = brayns::Json::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0].message, "read only property 'readOnly'");

        json = brayns::Json::parse(R"({})");
        errors = brayns::Json::validate(json, schema);
        CHECK(errors.empty());
    }
    SUBCASE("Item type")
    {
        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::Array;
        schema.items = {brayns::Json::getSchema<int>()};

        auto json = brayns::Json::parse(R"([1, 2, 3])");
        auto errors = brayns::Json::validate(json, schema);
        CHECK(errors.empty());

        json = brayns::Json::parse(R"([])");
        errors = brayns::Json::validate(json, schema);
        CHECK(errors.empty());

        json = brayns::Json::parse(R"([1, "test", 2])");
        errors = brayns::Json::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0].message, "invalid type, expected integer got string");
    }
    SUBCASE("Item count")
    {
        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::Array;
        schema.items = {brayns::Json::getSchema<int>()};
        schema.minItems = 1;
        schema.maxItems = 3;

        auto json = brayns::Json::parse(R"([1])");
        auto errors = brayns::Json::validate(json, schema);
        CHECK(errors.empty());

        json = brayns::Json::parse(R"([1, 2, 3])");
        errors = brayns::Json::validate(json, schema);
        CHECK(errors.empty());

        json = brayns::Json::parse(R"([])");
        errors = brayns::Json::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0].message, "item count below minimum 0 < 1");

        json = brayns::Json::parse(R"([1, 2, 3, 4])");
        errors = brayns::Json::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0].message, "item count above maximum 4 > 3");
    }
    SUBCASE("Nested")
    {
        auto internal = brayns::JsonSchema();
        internal.type = brayns::JsonType::Object;
        internal.properties["test3"] = brayns::Json::getSchema<std::vector<int>>();

        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::Object;
        auto integer = brayns::Json::getSchema<int>();
        integer.required = true;
        schema.properties["test1"] = std::move(integer);
        schema.properties["test2"] = internal;

        auto json = brayns::Json::parse(R"({"test2": {"test3": [1.3]}})");
        auto errors = brayns::Json::validate(json, schema);
        CHECK_EQ(errors.size(), 2);

        auto messages = brayns::JsonErrorFormatter::format(errors);
        CHECK_EQ(messages[0], "test2.test3[0]: invalid type, expected integer got number");
        CHECK_EQ(messages[1], "missing required property 'test1'");
    }
}
