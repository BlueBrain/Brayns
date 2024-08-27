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

#include <doctest.h>

#include <brayns/core/json/Json.h>

using namespace brayns;

TEST_CASE("Wildcard")
{
    auto schema = JsonSchema();
    auto json = parseJson(R"({"test": 10})");

    auto errors = validate(json, schema);
    CHECK(errors.empty());

    json = 1;
    errors = validate(json, schema);
    CHECK(errors.empty());
}

TEST_CASE("One of")
{
    auto schema = JsonSchema{.oneOf = {getJsonSchema<double>(), getJsonSchema<std::string>()}};

    auto errors = validate(1.0f, schema);
    CHECK(errors.empty());

    errors = validate("Test", schema);
    CHECK(errors.empty());

    errors = validate(true, schema);
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(toString(errors[0].error), "Invalid oneOf");
}

TEST_CASE("Invalid type")
{
    auto schema = JsonSchema{.type = JsonType::String};

    auto errors = validate(1, schema);
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(toString(errors[0].error), "Invalid type: expected string got integer");

    schema.type = JsonType::Number;
    errors = validate(1, schema);
    CHECK(errors.empty());
}

TEST_CASE("Limits")
{
    auto schema = JsonSchema{
        .type = JsonType::Integer,
        .minimum = -1,
        .maximum = 3,
    };

    auto errors = validate(1, schema);
    CHECK(errors.empty());

    errors = validate(-1, schema);
    CHECK(errors.empty());

    errors = validate(3, schema);
    CHECK(errors.empty());

    errors = validate(-2, schema);
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(toString(errors[0].error), "Value below minimum: -2 < -1");

    errors = validate(4, schema);
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(toString(errors[0].error), "Value above maximum: 4 > 3");

    schema.minimum = std::nullopt;
    schema.maximum = std::nullopt;

    errors = validate(-8, schema);
    CHECK(errors.empty());

    errors = validate(125, schema);
    CHECK(errors.empty());
}

TEST_CASE("Constant")
{
    auto schema = JsonSchema{
        .type = JsonType::String,
        .constant = "test",
    };

    auto errors = validate("test", schema);
    CHECK(errors.empty());

    errors = validate("test1", schema);
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(toString(errors[0].error), "Invalid const: expected 'test' got 'test1'");

    errors = validate(1, schema);
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(toString(errors[0].error), "Invalid type: expected string got integer");
}

TEST_CASE("Property type")
{
    auto schema = JsonSchema{
        .type = JsonType::Object,
        .properties = {{
            "internal",
            JsonSchema{
                .type = JsonType::Object,
                .properties = {{"integer", getJsonSchema<int>()}},
            },
        }}};

    auto json = parseJson(R"({"internal": 1})");
    auto errors = validate(json, schema);
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(toString(errors[0].error), "Invalid type: expected object got integer");

    json = parseJson(R"({"internal": {"integer": true}})");
    errors = validate(json, schema);
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(toString(errors[0].path), "internal.integer");
    CHECK_EQ(toString(errors[0].error), "Invalid type: expected integer got boolean");

    json = parseJson(R"({"internal": {"integer": 1}})");
    errors = validate(json, schema);
    CHECK(errors.empty());
}

TEST_CASE("Missing property")
{
    auto schema = JsonSchema{
        .type = JsonType::Object,
        .properties =
            {
                {"integer", JsonSchema{.type = JsonType::Integer}},
                {"string", JsonSchema{.required = false, .type = JsonType::String}},
            },
    };

    auto json = parseJson(R"({"integer": 1, "string": "test"})");
    auto errors = validate(json, schema);
    CHECK(errors.empty());

    json = parseJson(R"({"string": "test"})");
    errors = validate(json, schema);
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(toString(errors[0].error), "Missing required property: 'integer'");

    json = parseJson(R"({"integer": 1})");
    errors = validate(json, schema);
    CHECK(errors.empty());
}

TEST_CASE("Unknown properties")
{
    auto schema = JsonSchema{.type = JsonType::Object};

    auto json = parseJson(R"({"something": 1})");
    auto errors = validate(json, schema);
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(toString(errors[0].error), "Unknown property: 'something'");

    json = parseJson(R"({})");
    errors = validate(json, schema);
    CHECK(errors.empty());
}

TEST_CASE("Item type")
{
    auto schema = JsonSchema{
        .type = JsonType::Array,
        .items = {JsonSchema{.type = JsonType::Integer}},
    };

    auto json = parseJson(R"([1, 2, 3])");
    auto errors = validate(json, schema);
    CHECK(errors.empty());

    json = parseJson(R"([])");
    errors = validate(json, schema);
    CHECK(errors.empty());

    json = parseJson(R"([1, "test", 2])");
    errors = validate(json, schema);
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(toString(errors[0].path), "[1]");
    CHECK_EQ(toString(errors[0].error), "Invalid type: expected integer got string");
}

TEST_CASE("Unique items")
{
    auto schema = JsonSchema{
        .type = JsonType::Array,
        .items = {getJsonSchema<int>()},
        .uniqueItems = true,
    };

    auto json = parseJson(R"([1, 2, 3])");
    auto errors = validate(json, schema);
    CHECK(errors.empty());

    json = parseJson(R"([])");
    errors = validate(json, schema);
    CHECK(errors.empty());

    json = parseJson(R"([1, 2, 3, 2])");
    errors = validate(json, schema);
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(toString(errors[0].error), "Duplicated item: '2'");

    json = parseJson(R"([1, 1, 2, 2])");
    errors = validate(json, schema);
    CHECK_EQ(errors.size(), 2);
    CHECK_EQ(toString(errors[0].error), "Duplicated item: '1'");
    CHECK_EQ(toString(errors[1].error), "Duplicated item: '2'");
}

TEST_CASE("Item count")
{
    auto schema = JsonSchema{
        .type = JsonType::Array,
        .items = {getJsonSchema<int>()},
        .minItems = 1,
        .maxItems = 3,
    };

    auto json = parseJson(R"([1])");
    auto errors = validate(json, schema);
    CHECK(errors.empty());

    json = parseJson(R"([1, 2, 3])");
    errors = validate(json, schema);
    CHECK(errors.empty());

    json = parseJson(R"([])");
    errors = validate(json, schema);
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(toString(errors[0].error), "Not enough items: 0 < 1");

    json = parseJson(R"([1, 2, 3, 4])");
    errors = validate(json, schema);
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(toString(errors[0].error), "Too many items: 4 > 3");
}

TEST_CASE("Nested")
{
    auto internal = JsonSchema{
        .type = JsonType::Object,
        .properties = {{"test3", getJsonSchema<std::vector<int>>()}},
    };

    auto schema = JsonSchema{
        .type = JsonType::Object,
        .properties = {
            {"test1", JsonSchema{.required = true, .type = JsonType::Integer}},
            {"test2",
             JsonSchema{
                 .type = JsonType::Object,
                 .properties = {{"test3", getJsonSchema<std::vector<int>>()}},
             }},
        }};

    auto json = parseJson(R"({"test2": {"test3": [1.3]}})");
    auto errors = validate(json, schema);
    CHECK_EQ(errors.size(), 2);

    CHECK_EQ(toString(errors[0].path), "");
    CHECK_EQ(toString(errors[0].error), "Missing required property: 'test1'");

    CHECK_EQ(toString(errors[1].path), "test2.test3[0]");
    CHECK_EQ(toString(errors[1].error), "Invalid type: expected integer got number");
}

TEST_CASE("Schema as JSON")
{
    auto schema = getJsonSchema<std::string>();
    auto json = stringifyToJson(schema);
    auto ref = R"({"type":"string"})";
    CHECK_EQ(json, ref);

    schema = getJsonSchema<std::vector<std::string>>();
    json = stringifyToJson(schema);
    ref = R"({"items":{"type":"string"},"type":"array"})";
    CHECK_EQ(json, ref);

    schema = getJsonSchema<std::map<std::string, bool>>();
    json = stringifyToJson(schema);
    ref = R"({"additionalProperties":{"type":"boolean"},"type":"object"})";
    CHECK_EQ(json, ref);

    schema = getJsonSchema<std::variant<std::string, bool>>();
    json = stringifyToJson(schema);
    ref = R"({"oneOf":[{"type":"string"},{"type":"boolean"}]})";
    CHECK_EQ(json, ref);
}
