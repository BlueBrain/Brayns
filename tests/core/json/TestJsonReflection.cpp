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

namespace brayns
{
enum class SomeEnum
{
    Value1,
    Value2,
};

template<>
struct EnumReflector<SomeEnum>
{
    static auto reflect()
    {
        auto builder = EnumBuilder<SomeEnum>();
        builder.field("Value1", SomeEnum::Value1).description("Value 1");
        builder.field("Value2", SomeEnum::Value2).description("Value 2");
        return builder.build();
    }
};

struct Internal
{
    int value;
};

template<>
struct JsonObjectReflector<Internal>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Internal>();
        builder.description("Test child");
        builder.field("value", [](auto &object) { return &object.value; });
        return builder.build();
    }
};

struct SomeObject
{
    bool required;
    int bounded;
    bool description;
    std::string withDefault;
    std::optional<bool> optional;
    SomeEnum someEnum = SomeEnum::Value1;
    std::vector<int> array;
    Internal internal;
};

template<>
struct JsonObjectReflector<SomeObject>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<SomeObject>();
        builder.description("Test parent");
        builder.constant("constant", "test");
        builder.field("required", [](auto &object) { return &object.required; });
        builder.field("bounded", [](auto &object) { return &object.bounded; }).minimum(1).maximum(3);
        builder.field("description", [](auto &object) { return &object.description; }).description("Test");
        builder.field("default", [](auto &object) { return &object.withDefault; }).defaultValue("test");
        builder.field("optional", [](auto &object) { return &object.optional; });
        builder.field("enum", [](auto &object) { return &object.someEnum; });
        builder.field("array", [](auto &object) { return &object.array; }).minItems(1).maxItems(3);
        builder.field("internal", [](auto &object) { return &object.internal; });
        return builder.build();
    }
};

struct Updatable
{
    int value;
};

template<>
struct JsonObjectReflector<Updatable>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<Updatable>();
        builder.field("value", [](auto &object) { return &object.value; }).defaultValue(0);
        builder.removeDefaultValues();
        return builder.build();
    }
};
}

constexpr auto i32Min = std::numeric_limits<int>::lowest();
constexpr auto i32Max = std::numeric_limits<int>::max();
constexpr auto f32Min = std::numeric_limits<float>::lowest();
constexpr auto f32Max = std::numeric_limits<float>::max();

TEST_CASE("Undefined")
{
    CHECK_EQ(getJsonSchema<JsonValue>(), JsonSchema{.type = JsonType::Undefined});
    CHECK_EQ(deserializeJsonAs<JsonValue>(1), JsonValue(1));
    CHECK_EQ(serializeToJson(JsonValue("2")), JsonValue("2"));
}

TEST_CASE("Null")
{
    CHECK_EQ(getJsonSchema<NullJson>(), JsonSchema{.type = JsonType::Null});
    deserializeJsonAs<NullJson>({});
    CHECK_EQ(serializeToJson(NullJson()), JsonValue());
    CHECK_THROWS_AS(deserializeJsonAs<NullJson>("xyz"), JsonException);
}

TEST_CASE("Boolean")
{
    CHECK_EQ(getJsonSchema<bool>(), JsonSchema{.type = JsonType::Boolean});
    CHECK_EQ(deserializeJsonAs<bool>(true), true);
    CHECK_EQ(serializeToJson(true), JsonValue(true));
    CHECK_THROWS_AS(deserializeJsonAs<bool>("xyz"), JsonException);
}

TEST_CASE("Integer")
{
    CHECK_EQ(getJsonSchema<std::uint8_t>(), JsonSchema{.type = JsonType::Integer, .minimum = 0, .maximum = 255});
    CHECK_EQ(getJsonSchema<std::int16_t>().type, JsonType::Integer);
    CHECK_EQ(getJsonSchema<int>().type, JsonType::Integer);
    CHECK_EQ(deserializeJsonAs<int>(1), 1);
    CHECK_EQ(serializeToJson(1), JsonValue(1));
    CHECK_THROWS_AS(deserializeJsonAs<int>(1.5), JsonException);
}

TEST_CASE("Number")
{
    CHECK_EQ(getJsonSchema<float>(), JsonSchema{.type = JsonType::Number, .minimum = f32Min, .maximum = f32Max});
    CHECK_EQ(getJsonSchema<double>().type, JsonType::Number);
    CHECK_EQ(deserializeJsonAs<float>(1), 1.0f);
    CHECK_EQ(serializeToJson(1.5f), JsonValue(1.5f));
    CHECK_THROWS_AS(deserializeJsonAs<float>("1.5"), JsonException);
}

TEST_CASE("String")
{
    CHECK_EQ(getJsonSchema<std::string>(), JsonSchema{.type = JsonType::String});
    CHECK_EQ(deserializeJsonAs<std::string>("test"), JsonValue("test"));
    CHECK_EQ(serializeToJson(std::string("test")), JsonValue("test"));
    CHECK_THROWS_AS(deserializeJsonAs<std::string>(1), JsonException);
}

TEST_CASE("Const")
{
    CHECK_EQ(getJsonSchema<JsonFalse>(), JsonSchema{.type = JsonType::Boolean, .constant = false});
    CHECK_EQ(deserializeJsonAs<JsonFalse>(false), JsonFalse());
    CHECK_EQ(serializeToJson(JsonFalse()), false);
    CHECK_THROWS_AS(deserializeJsonAs<JsonFalse>(1), JsonException);
}

TEST_CASE("Enum")
{
    CHECK_EQ(
        getJsonSchema<SomeEnum>(),
        JsonSchema{
            .oneOf = {
                JsonSchema{
                    .description = "Value 1",
                    .type = JsonType::String,
                    .constant = "Value1",
                },
                JsonSchema{
                    .description = "Value 2",
                    .type = JsonType::String,
                    .constant = "Value2",
                },
            }});
    CHECK_EQ(deserializeJsonAs<SomeEnum>("Value1"), SomeEnum::Value1);
    CHECK_EQ(serializeToJson(SomeEnum::Value2), JsonValue("Value2"));
    CHECK_THROWS_AS(deserializeJsonAs<SomeEnum>(1), JsonException);
    CHECK_THROWS_AS(deserializeJsonAs<SomeEnum>("Value3"), JsonException);
}

TEST_CASE("Array")
{
    CHECK_EQ(
        getJsonSchema<std::vector<std::string>>(),
        JsonSchema{
            .type = JsonType::Array,
            .items = {JsonSchema{.type = JsonType::String}},
        });
    CHECK_EQ(parseJsonAs<std::vector<int>>("[1,2,3]"), std::vector<int>{1, 2, 3});
    CHECK_EQ(stringifyToJson(std::vector<int>{1, 2, 3}), "[1,2,3]");
}

TEST_CASE("Set")
{
    CHECK_EQ(
        getJsonSchema<std::set<std::string>>(),
        JsonSchema{
            .type = JsonType::Array,
            .items = {JsonSchema{.type = JsonType::String}},
            .uniqueItems = true,
        });
    CHECK_EQ(parseJsonAs<std::set<int>>("[1,2,3]"), std::set<int>{1, 2, 3});
    CHECK_EQ(stringifyToJson(std::set<int>{1, 2, 3}), "[1,2,3]");
    CHECK_THROWS_AS(parseJsonAs<std::set<int>>("[1,2,2]"), JsonException);
}

TEST_CASE("Math")
{
    CHECK_EQ(
        getJsonSchema<Vector3>(),
        JsonSchema{
            .type = JsonType::Array,
            .items = {JsonSchema{.type = JsonType::Number, .minimum = f32Min, .maximum = f32Max}},
            .minItems = 3,
            .maxItems = 3,
        });

    CHECK_EQ(
        getJsonSchema<Quaternion>(),
        JsonSchema{
            .type = JsonType::Array,
            .items = {JsonSchema{.type = JsonType::Number, .minimum = f32Min, .maximum = f32Max}},
            .minItems = 4,
            .maxItems = 4,
        });

    CHECK_EQ(parseJsonAs<Vector3>("[1,2,3]"), Vector3(1, 2, 3));
    CHECK_EQ(parseJsonAs<Quaternion>("[1,2,3,4]"), Quaternion(4, 1, 2, 3));

    CHECK_EQ(stringifyToJson(Vector3(1, 2, 3)), "[1,2,3]");
    CHECK_EQ(stringifyToJson(Quaternion(4, 1, 2, 3)), "[1,2,3,4]");

    CHECK_THROWS_AS(parseJsonAs<Vector3>("[1,2,3,4]"), JsonException);
    CHECK_THROWS_AS(parseJsonAs<Quaternion>("[1,2,3,4,5]"), JsonException);

    CHECK_THROWS_AS(parseJsonAs<Vector3>("[1,2]"), JsonException);
    CHECK_THROWS_AS(parseJsonAs<Quaternion>("[1,2]"), JsonException);
}

TEST_CASE("Map")
{
    using Map = std::map<std::string, int>;

    CHECK_EQ(
        getJsonSchema<std::map<std::string, std::string>>(),
        JsonSchema{
            .type = JsonType::Object,
            .items = {JsonSchema{.type = JsonType::String}},
        });

    auto map = Map{{"test1", 1}, {"test2", 2}};
    auto json = R"({"test1":1,"test2":2})";

    CHECK_EQ(parseJsonAs<Map>(json), map);
    CHECK_EQ(stringifyToJson(map), json);

    CHECK_THROWS_AS(parseJsonAs<Map>(R"({"invalid":2.5})"), JsonException);
}

TEST_CASE("Variant")
{
    using Variant = std::variant<std::string, int>;

    CHECK_EQ(
        getJsonSchema<Variant>(),
        JsonSchema{
            .oneOf = {
                JsonSchema{.type = JsonType::String},
                JsonSchema{.type = JsonType::Integer, .minimum = i32Min, .maximum = i32Max},
            }});
    CHECK_EQ(serializeToJson(Variant("test")), JsonValue("test"));
    CHECK_EQ(serializeToJson(Variant(1)), JsonValue(1));
    CHECK_EQ(deserializeJsonAs<Variant>(1), Variant(1));
    CHECK_EQ(deserializeJsonAs<Variant>("test"), Variant("test"));
    CHECK_THROWS_AS(deserializeJsonAs<Variant>(1.5), JsonException);

    CHECK_EQ(
        getJsonSchema<std::optional<std::string>>(),
        JsonSchema{
            .oneOf = {JsonSchema{.type = JsonType::String}, JsonSchema{.type = JsonType::Null}},
        });
    CHECK_EQ(serializeToJson(std::optional<std::string>("test")), JsonValue("test"));
    CHECK_EQ(serializeToJson(std::optional<std::string>()), JsonValue());
    CHECK_EQ(deserializeJsonAs<std::optional<std::string>>({}), std::nullopt);
    CHECK_EQ(deserializeJsonAs<std::optional<std::string>>("test"), std::string("test"));
    CHECK_THROWS_AS(deserializeJsonAs<std::optional<std::string>>(1.5), JsonException);
}

TEST_CASE("Object")
{
    const auto &schema = getJsonSchema<SomeObject>();

    CHECK_EQ(schema.description, "Test parent");
    CHECK_EQ(schema.type, JsonType::Object);

    const auto &properties = schema.properties;

    CHECK_EQ(properties.at("constant"), JsonSchema{.type = JsonType::String, .constant = "test"});
    CHECK_EQ(properties.at("required"), getJsonSchema<bool>());
    CHECK_EQ(properties.at("bounded"), JsonSchema{.type = JsonType::Integer, .minimum = 1, .maximum = 3});
    CHECK_EQ(properties.at("description"), JsonSchema{.description = "Test", .type = JsonType::Boolean});
    CHECK_EQ(properties.at("default"), JsonSchema{.required = false, .defaultValue = "test", .type = JsonType::String});
    CHECK_EQ(properties.at("optional"), getJsonSchema<std::optional<bool>>());
    CHECK_EQ(properties.at("enum"), getJsonSchema<SomeEnum>());
    CHECK_EQ(
        properties.at("array"),
        JsonSchema{
            .type = JsonType::Array,
            .items = {getJsonSchema<int>()},
            .minItems = 1,
            .maxItems = 3,
        });
    CHECK_EQ(
        properties.at("internal"),
        JsonSchema{
            .description = "Test child",
            .type = JsonType::Object,
            .properties = {{"value", getJsonSchema<int>()}},
        });

    auto internal = createJsonObject();
    internal->set("value", 2);

    auto object = createJsonObject();
    object->set("constant", "test");
    object->set("required", true);
    object->set("bounded", 2);
    object->set("description", true);
    object->set("optional", JsonValue());
    object->set("enum", "Value2");
    object->set("array", serializeToJson(std::vector<int>{1, 2, 3}));
    object->set("internal", internal);

    auto json = JsonValue(object);

    auto test = deserializeJsonAs<SomeObject>(json);

    CHECK(test.required);
    CHECK_EQ(test.bounded, 2);
    CHECK(test.description);
    CHECK_EQ(test.withDefault, "test");
    CHECK_FALSE(test.optional);
    CHECK_EQ(test.someEnum, SomeEnum::Value2);
    CHECK_EQ(test.array, std::vector<int>{1, 2, 3});
    CHECK_EQ(test.internal.value, 2);

    object->set("default", "test");

    CHECK_EQ(stringifyToJson(test), stringifyToJson(json));
}

TEST_CASE("Empty field")
{
    auto object = Updatable{1};

    auto json = JsonValue(createJsonObject());

    deserializeJson(json, object);

    CHECK_EQ(object.value, 1);
}
