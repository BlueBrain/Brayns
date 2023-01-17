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

#include <brayns/json/JsonType.h>

TEST_CASE("JsonTypeHelper")
{
    SUBCASE("check")
    {
        CHECK(brayns::JsonTypeHelper::check(brayns::JsonType::Array, brayns::JsonType::Array));
        CHECK(brayns::JsonTypeHelper::check(brayns::JsonType::Number, brayns::JsonType::Integer));
        CHECK_FALSE(brayns::JsonTypeHelper::check(brayns::JsonType::Integer, brayns::JsonType::Number));
        CHECK_FALSE(brayns::JsonTypeHelper::check(brayns::JsonType::Object, brayns::JsonType::String));
    }
    SUBCASE("isNumeric")
    {
        CHECK_FALSE(brayns::JsonTypeHelper::isNumeric(brayns::JsonType::Undefined));
        CHECK_FALSE(brayns::JsonTypeHelper::isNumeric(brayns::JsonType::Null));
        CHECK_FALSE(brayns::JsonTypeHelper::isNumeric(brayns::JsonType::Boolean));
        CHECK(brayns::JsonTypeHelper::isNumeric(brayns::JsonType::Integer));
        CHECK(brayns::JsonTypeHelper::isNumeric(brayns::JsonType::Number));
        CHECK_FALSE(brayns::JsonTypeHelper::isNumeric(brayns::JsonType::String));
        CHECK_FALSE(brayns::JsonTypeHelper::isNumeric(brayns::JsonType::Array));
        CHECK_FALSE(brayns::JsonTypeHelper::isNumeric(brayns::JsonType::Object));
    }
}

TEST_CASE("GetJsonType")
{
    SUBCASE("fromJson")
    {
        struct NotJson
        {
        };
        CHECK_EQ(brayns::GetJsonType::fromJson(NotJson()), brayns::JsonType::Undefined);
        CHECK_EQ(brayns::GetJsonType::fromJson(brayns::JsonValue()), brayns::JsonType::Null);
        CHECK_EQ(brayns::GetJsonType::fromJson(true), brayns::JsonType::Boolean);
        CHECK_EQ(brayns::GetJsonType::fromJson(std::int16_t(1)), brayns::JsonType::Integer);
        CHECK_EQ(brayns::GetJsonType::fromJson(std::uint32_t(1)), brayns::JsonType::Integer);
        CHECK_EQ(brayns::GetJsonType::fromJson(1.0f), brayns::JsonType::Number);
        CHECK_EQ(brayns::GetJsonType::fromJson(1.0), brayns::JsonType::Number);
        CHECK_EQ(brayns::GetJsonType::fromJson(std::string("Test")), brayns::JsonType::String);
        CHECK_EQ(brayns::GetJsonType::fromJson(Poco::makeShared<brayns::JsonArray>()), brayns::JsonType::Array);
        CHECK_EQ(brayns::GetJsonType::fromJson(Poco::makeShared<brayns::JsonObject>()), brayns::JsonType::Object);
    }
    SUBCASE("fromPrimitive")
    {
        CHECK_EQ(brayns::GetJsonType::fromPrimitive<std::nullptr_t>(), brayns::JsonType::Null);
        CHECK_EQ(brayns::GetJsonType::fromPrimitive<bool>(), brayns::JsonType::Boolean);
        CHECK_EQ(brayns::GetJsonType::fromPrimitive<std::int16_t>(), brayns::JsonType::Integer);
        CHECK_EQ(brayns::GetJsonType::fromPrimitive<std::uint32_t>(), brayns::JsonType::Integer);
        CHECK_EQ(brayns::GetJsonType::fromPrimitive<float>(), brayns::JsonType::Number);
        CHECK_EQ(brayns::GetJsonType::fromPrimitive<double>(), brayns::JsonType::Number);
        CHECK_EQ(brayns::GetJsonType::fromPrimitive<std::string>(), brayns::JsonType::String);
    }
}
