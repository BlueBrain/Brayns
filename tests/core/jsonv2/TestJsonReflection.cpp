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

#include <brayns/core/jsonv2/Json.h>

using namespace brayns;
using namespace brayns::experimental;

namespace brayns::experimental
{
enum class SomeEnum
{
    Value1,
    Value2,
};

template<>
struct EnumReflector<SomeEnum>
{
    static EnumInfo<SomeEnum> reflect()
    {
        auto builder = EnumInfoBuilder<SomeEnum>();
        builder.field("value1", SomeEnum::Value1).description("Value 1");
        builder.field("value2", SomeEnum::Value2).description("Value 2");
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
    static JsonObjectInfo<Internal> reflect()
    {
        auto builder = JsonObjectInfoBuilder<Internal>();
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
    static JsonObjectInfo<SomeObject> reflect()
    {
        auto builder = JsonObjectInfoBuilder<SomeObject>();
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
}

TEST_CASE("JsonReflection")
{
    constexpr auto imin = std::numeric_limits<int>::lowest();
    constexpr auto imax = std::numeric_limits<int>::max();
    constexpr auto fmin = std::numeric_limits<float>::lowest();
    constexpr auto fmax = std::numeric_limits<float>::max();

    SUBCASE("Undefined")
    {
        CHECK_EQ(getJsonSchema<JsonValue>(), JsonSchema{.type = JsonType::Undefined});
        CHECK_EQ(deserializeAs<JsonValue>(1), JsonValue(1));
        CHECK_EQ(serializeToJson(JsonValue("2")), JsonValue("2"));
    }
    SUBCASE("Null")
    {
        CHECK_EQ(getJsonSchema<NullJson>(), JsonSchema{.type = JsonType::Null});
        deserializeAs<NullJson>({});
        CHECK_EQ(serializeToJson(NullJson()), JsonValue());
        CHECK_THROWS_AS(deserializeAs<NullJson>("xyz"), JsonException);
    }
    SUBCASE("Boolean")
    {
        CHECK_EQ(getJsonSchema<bool>(), JsonSchema{.type = JsonType::Boolean});
        CHECK_EQ(deserializeAs<bool>(true), true);
        CHECK_EQ(serializeToJson(true), JsonValue(true));
        CHECK_THROWS_AS(deserializeAs<bool>("xyz"), JsonException);
    }
    SUBCASE("Integer")
    {
        CHECK_EQ(getJsonSchema<std::uint8_t>(), JsonSchema{.type = JsonType::Integer, .minimum = 0, .maximum = 255});
        CHECK_EQ(getJsonSchema<std::int16_t>().type, JsonType::Integer);
        CHECK_EQ(getJsonSchema<int>().type, JsonType::Integer);
        CHECK_EQ(deserializeAs<int>(1), 1);
        CHECK_EQ(serializeToJson(1), JsonValue(1));
        CHECK_THROWS_AS(deserializeAs<int>(1.5), JsonException);
    }
    SUBCASE("Number")
    {
        CHECK_EQ(getJsonSchema<float>(), JsonSchema{.type = JsonType::Number, .minimum = fmin, .maximum = fmax});
        CHECK_EQ(getJsonSchema<double>().type, JsonType::Number);
        CHECK_EQ(deserializeAs<float>(1), 1.0f);
        CHECK_EQ(serializeToJson(1.5f), JsonValue(1.5f));
        CHECK_THROWS_AS(deserializeAs<float>("1.5"), JsonException);
    }
    SUBCASE("String")
    {
        CHECK_EQ(getJsonSchema<std::string>(), JsonSchema{.type = JsonType::String});
        CHECK_EQ(deserializeAs<std::string>("test"), JsonValue("test"));
        CHECK_EQ(serializeToJson(std::string("test")), JsonValue("test"));
        CHECK_THROWS_AS(deserializeAs<std::string>(1), JsonException);
    }
    SUBCASE("Enum")
    {
        CHECK_EQ(
            getJsonSchema<SomeEnum>(),
            JsonSchema{
                .oneOf = {
                    JsonSchema{
                        .description = "Value 1",
                        .type = JsonType::String,
                        .constant = "value1",
                    },
                    JsonSchema{
                        .description = "Value 2",
                        .type = JsonType::String,
                        .constant = "value2",
                    },
                }});
        CHECK_EQ(deserializeAs<SomeEnum>("value1"), SomeEnum::Value1);
        CHECK_EQ(serializeToJson(SomeEnum::Value2), JsonValue("value2"));
        CHECK_THROWS_AS(deserializeAs<SomeEnum>(1), JsonException);
        CHECK_THROWS_AS(deserializeAs<SomeEnum>("value3"), JsonException);
    }
    SUBCASE("Array")
    {
        CHECK_EQ(
            getJsonSchema<std::vector<std::string>>(),
            JsonSchema{.type = JsonType::Array, .items = {JsonSchema{.type = JsonType::String}}});
        CHECK_EQ(parseJson<std::vector<int>>("[1,2,3]"), std::vector<int>{1, 2, 3});
        CHECK_EQ(stringifyToJson(std::vector<int>{1, 2, 3}), "[1,2,3]");
    }
    SUBCASE("Math")
    {
        CHECK_EQ(
            getJsonSchema<Vector3>(),
            JsonSchema{
                .type = JsonType::Array,
                .items = {JsonSchema{.type = JsonType::Number, .minimum = fmin, .maximum = fmax}},
                .minItems = 3,
                .maxItems = 3,
            });

        CHECK_EQ(
            getJsonSchema<Quaternion>(),
            JsonSchema{
                .type = JsonType::Array,
                .items = {JsonSchema{.type = JsonType::Number, .minimum = fmin, .maximum = fmax}},
                .minItems = 4,
                .maxItems = 4,
            });

        CHECK_EQ(parseJson<Vector3>("[1,2,3]"), Vector3(1, 2, 3));
        CHECK_EQ(parseJson<Quaternion>("[1,2,3,4]"), Quaternion(4, 1, 2, 3));

        CHECK_EQ(stringifyToJson(Vector3(1, 2, 3)), "[1,2,3]");
        CHECK_EQ(stringifyToJson(Quaternion(4, 1, 2, 3)), "[1,2,3,4]");

        CHECK_THROWS_AS(parseJson<Vector3>("[1,2,3,4]"), JsonException);
        CHECK_THROWS_AS(parseJson<Quaternion>("[1,2,3,4,5]"), JsonException);

        CHECK_THROWS_AS(parseJson<Vector3>("[1,2]"), JsonException);
        CHECK_THROWS_AS(parseJson<Quaternion>("[1,2]"), JsonException);
    }
    SUBCASE("Map")
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

        CHECK_EQ(parseJson<Map>(json), map);
        CHECK_EQ(stringifyToJson(map), json);

        CHECK_THROWS_AS(parseJson<Map>(R"({"invalid":2.5})"), JsonException);
    }
    SUBCASE("Variant")
    {
        using Variant = std::variant<std::string, int>;

        CHECK_EQ(
            getJsonSchema<Variant>(),
            JsonSchema{
                .oneOf = {
                    JsonSchema{.type = JsonType::String},
                    JsonSchema{.type = JsonType::Integer, .minimum = imin, .maximum = imax},
                }});
        CHECK_EQ(serializeToJson(Variant("test")), JsonValue("test"));
        CHECK_EQ(serializeToJson(Variant(1)), JsonValue(1));
        CHECK_EQ(deserializeAs<Variant>(1), Variant(1));
        CHECK_EQ(deserializeAs<Variant>("test"), Variant("test"));
        CHECK_THROWS_AS(deserializeAs<Variant>(1.5), JsonException);

        CHECK_EQ(
            getJsonSchema<std::optional<std::string>>(),
            JsonSchema{
                .required = false,
                .oneOf = {JsonSchema{.type = JsonType::String}, JsonSchema{.type = JsonType::Null}},
            });
        CHECK_EQ(serializeToJson(std::optional<std::string>("test")), JsonValue("test"));
        CHECK_EQ(serializeToJson(std::optional<std::string>()), JsonValue());
        CHECK_EQ(deserializeAs<std::optional<std::string>>({}), std::nullopt);
        CHECK_EQ(deserializeAs<std::optional<std::string>>("test"), std::string("test"));
        CHECK_THROWS_AS(deserializeAs<std::optional<std::string>>(1.5), JsonException);
    }
    SUBCASE("Object")
    {
        auto schema = getJsonSchema<SomeObject>();

        CHECK_EQ(schema.type, JsonType::Object);

        const auto &properties = schema.properties;

        CHECK_EQ(properties.at("required"), getJsonSchema<bool>());
        CHECK_EQ(properties.at("bounded"), JsonSchema{.type = JsonType::Integer, .minimum = 1, .maximum = 3});
        CHECK_EQ(properties.at("description"), JsonSchema{.description = "Test", .type = JsonType::Boolean});
        CHECK_EQ(
            properties.at("default"),
            JsonSchema{.required = false, .defaultValue = "test", .type = JsonType::String});
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
                .type = JsonType::Object,
                .properties = {{"value", getJsonSchema<int>()}},
            });

        auto internal = createJsonObject();
        internal->set("value", 2);

        auto object = createJsonObject();
        object->set("required", true);
        object->set("bounded", 2);
        object->set("description", true);
        object->set("enum", "value2");
        object->set("array", serializeToJson(std::vector<int>{1, 2, 3}));
        object->set("internal", internal);

        auto json = JsonValue(object);

        auto test = deserializeAs<SomeObject>(json);

        CHECK(test.required);
        CHECK_EQ(test.bounded, 2);
        CHECK(test.description);
        CHECK_EQ(test.withDefault, "test");
        CHECK_FALSE(test.optional);
        CHECK_EQ(test.someEnum, SomeEnum::Value2);
        CHECK_EQ(test.array, std::vector<int>{1, 2, 3});
        CHECK_EQ(test.internal.value, 2);

        object->set("default", "test");

        auto backToJson = serializeToJson(test);

        CHECK_EQ(backToJson, json);
    }
}
