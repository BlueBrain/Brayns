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

enum class SomeEnum
{
    Value1,
    Value2,
};

namespace brayns::experimental
{
template<>
struct EnumReflector<SomeEnum>
{
    static EnumInfo<SomeEnum> reflect()
    {
        return {
            {"value1", SomeEnum::Value1},
            {"value2", SomeEnum::Value2},
        };
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
        CHECK_EQ(deserializeJson<JsonValue>(1), JsonValue(1));
        CHECK_EQ(serializeToJson(JsonValue("2")), JsonValue("2"));
    }
    SUBCASE("Null")
    {
        CHECK_EQ(getJsonSchema<NullJson>(), JsonSchema{.type = JsonType::Null});
        deserializeJson<NullJson>({});
        CHECK_EQ(serializeToJson(NullJson()), JsonValue());
        CHECK_THROWS_AS(deserializeJson<NullJson>("xyz"), JsonException);
    }
    SUBCASE("Boolean")
    {
        CHECK_EQ(getJsonSchema<bool>(), JsonSchema{.type = JsonType::Boolean});
        CHECK_EQ(deserializeJson<bool>(true), true);
        CHECK_EQ(serializeToJson(true), JsonValue(true));
        CHECK_THROWS_AS(deserializeJson<bool>("xyz"), JsonException);
    }
    SUBCASE("Integer")
    {
        CHECK_EQ(getJsonSchema<std::uint8_t>(), JsonSchema{.type = JsonType::Integer, .minimum = 0, .maximum = 255});
        CHECK_EQ(getJsonSchema<std::int16_t>().type, JsonType::Integer);
        CHECK_EQ(getJsonSchema<int>().type, JsonType::Integer);
        CHECK_EQ(deserializeJson<int>(1), 1);
        CHECK_EQ(serializeToJson(1), JsonValue(1));
        CHECK_THROWS_AS(deserializeJson<int>(1.5), JsonException);
    }
    SUBCASE("Number")
    {
        CHECK_EQ(getJsonSchema<float>(), JsonSchema{.type = JsonType::Number, .minimum = fmin, .maximum = fmax});
        CHECK_EQ(getJsonSchema<double>().type, JsonType::Number);
        CHECK_EQ(deserializeJson<float>(1), 1.0f);
        CHECK_EQ(serializeToJson(1.5f), JsonValue(1.5f));
        CHECK_THROWS_AS(deserializeJson<float>("1.5"), JsonException);
    }
    SUBCASE("String")
    {
        CHECK_EQ(getJsonSchema<std::string>(), JsonSchema{.type = JsonType::String});
        CHECK_EQ(deserializeJson<std::string>("test"), JsonValue("test"));
        CHECK_EQ(serializeToJson(std::string("test")), JsonValue("test"));
        CHECK_THROWS_AS(deserializeJson<std::string>(1), JsonException);
    }
    SUBCASE("Enum")
    {
        CHECK_EQ(getJsonSchema<SomeEnum>(), JsonSchema{.type = JsonType::String, .enums = {"value1", "value2"}});
        CHECK_EQ(deserializeJson<SomeEnum>("value1"), SomeEnum::Value1);
        CHECK_EQ(serializeToJson(SomeEnum::Value2), JsonValue("value2"));
        CHECK_THROWS_AS(deserializeJson<SomeEnum>(1), JsonException);
        CHECK_THROWS_AS(deserializeJson<SomeEnum>("value3"), JsonException);
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
        CHECK_EQ(deserializeJson<Variant>(1), Variant(1));
        CHECK_EQ(deserializeJson<Variant>("test"), Variant("test"));
        CHECK_THROWS_AS(deserializeJson<Variant>(1.5), JsonException);

        CHECK_EQ(
            getJsonSchema<std::optional<std::string>>(),
            JsonSchema{
                .required = false,
                .oneOf = {JsonSchema{.type = JsonType::String}, JsonSchema{.type = JsonType::Null}},
            });
        CHECK_EQ(serializeToJson(std::optional<std::string>("test")), JsonValue("test"));
        CHECK_EQ(serializeToJson(std::optional<std::string>()), JsonValue());
        CHECK_EQ(deserializeJson<std::optional<std::string>>({}), std::nullopt);
        CHECK_EQ(deserializeJson<std::optional<std::string>>("test"), std::string("test"));
        CHECK_THROWS_AS(deserializeJson<std::optional<std::string>>(1.5), JsonException);
    }
}
