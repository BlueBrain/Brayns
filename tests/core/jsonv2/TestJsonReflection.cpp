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
        constexpr auto fmin = std::numeric_limits<float>::lowest();
        constexpr auto fmax = std::numeric_limits<float>::max();
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
    SUBCASE("Enums")
    {
        CHECK_EQ(getJsonSchema<SomeEnum>(), JsonSchema{.type = JsonType::String, .enums = {"value1", "value2"}});
        CHECK_EQ(deserializeJson<SomeEnum>("value1"), SomeEnum::Value1);
        CHECK_EQ(serializeToJson(SomeEnum::Value2), JsonValue("value2"));
        CHECK_THROWS_AS(deserializeJson<SomeEnum>(1), JsonException);
        CHECK_THROWS_AS(deserializeJson<SomeEnum>("value3"), JsonException);
    }
}
