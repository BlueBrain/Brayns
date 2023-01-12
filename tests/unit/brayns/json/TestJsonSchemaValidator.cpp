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

#include <brayns/json/Json.h>
#include <brayns/json/JsonSchemaValidator.h>
#include <brayns/json/adapters/PrimitiveAdapter.h>

TEST_CASE("JsonSchemaValidator")
{
    SUBCASE("Wildcard")
    {
        auto schema = brayns::JsonSchemaHelper::getWildcardSchema();
        auto json = brayns::Json::parse(R"({"test": 10})");
        auto errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK(errors.empty());
        json = 1;
        errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK(errors.empty());
    }
    SUBCASE("One of")
    {
        auto schema = brayns::JsonSchema();
        schema.oneOf = {brayns::Json::getSchema<double>(), brayns::Json::getSchema<std::string>()};
        auto errors = brayns::JsonSchemaValidator::validate(1.0f, schema);
        CHECK(errors.empty());
        errors = brayns::JsonSchemaValidator::validate("Test", schema);
        CHECK(errors.empty());
        errors = brayns::JsonSchemaValidator::validate(true, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0], "Cannot find a schema in oneOf that matches the given input");
    }
    SUBCASE("Invalid type")
    {
        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::String;
        auto errors = brayns::JsonSchemaValidator::validate(1, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0], "Invalid type: expected 'string', got 'integer'");
        schema.type = brayns::JsonType::Number;
        errors = brayns::JsonSchemaValidator::validate(1, schema);
        CHECK(errors.empty());
    }
    SUBCASE("Limits")
    {
        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::Integer;
        schema.minimum = -1;
        schema.maximum = 3;
        auto errors = brayns::JsonSchemaValidator::validate(1, schema);
        CHECK(errors.empty());
        errors = brayns::JsonSchemaValidator::validate(-1, schema);
        CHECK(errors.empty());
        errors = brayns::JsonSchemaValidator::validate(3, schema);
        CHECK(errors.empty());
        errors = brayns::JsonSchemaValidator::validate(-2, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0], "Value below minimum: expected >= -1, got -2");
        errors = brayns::JsonSchemaValidator::validate(4, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0], "Value above maximum: expected <= 3, got 4");
    }
    SUBCASE("Enums")
    {
        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::String;
        schema.enums = {"test1", "test2"};
        auto errors = brayns::JsonSchemaValidator::validate("test1", schema);
        CHECK(errors.empty());
        errors = brayns::JsonSchemaValidator::validate("test2", schema);
        CHECK(errors.empty());
        errors = brayns::JsonSchemaValidator::validate("Test2", schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0], "Invalid enum: 'Test2' not in ['test1', 'test2']");
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
        auto errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0], "Invalid type for internal: expected 'object', got 'integer'");
        json = brayns::Json::parse(R"({"internal": {"integer": true}})");
        errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0], "Invalid type for internal.integer: expected 'integer', got 'boolean'");
        json = brayns::Json::parse(R"({"internal": {"integer": 1}})");
        errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK(errors.empty());
    }
    SUBCASE("Missing property")
    {
        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::Object;
        schema.properties = {
            {"integer", brayns::Json::getSchema<int>()},
            {"string", brayns::Json::getSchema<std::string>()}};
        schema.required = {"integer"};
        auto json = brayns::Json::parse(R"({"integer": 1, "string": "test"})");
        auto errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK(errors.empty());
        json = brayns::Json::parse(R"({"string": "test"})");
        errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0], "Missing property: 'integer'");
        json = brayns::Json::parse(R"({"integer": 1})");
        errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK(errors.empty());
    }
    SUBCASE("Additional properties")
    {
        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::Object;
        auto json = brayns::Json::parse(R"({"something": 1})");
        auto errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0], "Unknown property: 'something'");
        brayns::JsonSchemaHelper::allowAnyAdditionalProperty(schema);
        errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK(errors.empty());
    }
    SUBCASE("Item type")
    {
        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::Array;
        schema.items = {brayns::Json::getSchema<int>()};
        auto json = brayns::Json::parse(R"([1, 2, 3])");
        auto errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK(errors.empty());
        json = brayns::Json::parse(R"([])");
        errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK(errors.empty());
        json = brayns::Json::parse(R"([1, "test", 2])");
        errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0], "Invalid type for [1]: expected 'integer', got 'string'");
    }
    SUBCASE("Item count")
    {
        auto schema = brayns::JsonSchema();
        schema.type = brayns::JsonType::Array;
        schema.items = {brayns::Json::getSchema<int>()};
        schema.minItems = 1;
        schema.maxItems = 3;
        auto json = brayns::Json::parse(R"([1])");
        auto errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK(errors.empty());
        json = brayns::Json::parse(R"([1, 2, 3])");
        errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK(errors.empty());
        json = brayns::Json::parse(R"([])");
        errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0], "Not enough items: expected at least 1 item(s), got 0");
        json = brayns::Json::parse(R"([1, 2, 3, 4])");
        errors = brayns::JsonSchemaValidator::validate(json, schema);
        CHECK_EQ(errors.size(), 1);
        CHECK_EQ(errors[0], "Too many items: expected at most 3 item(s), got 4");
    }
}
