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

#include <brayns/core/json/JsonType.h>

TEST_CASE("JsonTypeInfo")
{
    SUBCASE("isNumeric")
    {
        CHECK_FALSE(brayns::JsonTypeInfo::isNumeric(brayns::JsonType::Undefined));
        CHECK_FALSE(brayns::JsonTypeInfo::isNumeric(brayns::JsonType::Null));
        CHECK_FALSE(brayns::JsonTypeInfo::isNumeric(brayns::JsonType::Boolean));
        CHECK(brayns::JsonTypeInfo::isNumeric(brayns::JsonType::Integer));
        CHECK(brayns::JsonTypeInfo::isNumeric(brayns::JsonType::Number));
        CHECK_FALSE(brayns::JsonTypeInfo::isNumeric(brayns::JsonType::String));
        CHECK_FALSE(brayns::JsonTypeInfo::isNumeric(brayns::JsonType::Array));
        CHECK_FALSE(brayns::JsonTypeInfo::isNumeric(brayns::JsonType::Object));
    }
    SUBCASE("getType")
    {
        struct NotJson
        {
        };

        CHECK_THROWS_AS(brayns::JsonTypeInfo::getType(NotJson()), std::invalid_argument);
        CHECK_EQ(brayns::JsonTypeInfo::getType(brayns::JsonValue()), brayns::JsonType::Null);
        CHECK_EQ(brayns::JsonTypeInfo::getType(true), brayns::JsonType::Boolean);
        CHECK_EQ(brayns::JsonTypeInfo::getType(std::int16_t(1)), brayns::JsonType::Integer);
        CHECK_EQ(brayns::JsonTypeInfo::getType(std::uint32_t(1)), brayns::JsonType::Integer);
        CHECK_EQ(brayns::JsonTypeInfo::getType(1.0f), brayns::JsonType::Number);
        CHECK_EQ(brayns::JsonTypeInfo::getType(1.0), brayns::JsonType::Number);
        CHECK_EQ(brayns::JsonTypeInfo::getType(std::string("Test")), brayns::JsonType::String);
        CHECK_EQ(brayns::JsonTypeInfo::getType(Poco::makeShared<brayns::JsonArray>()), brayns::JsonType::Array);
        CHECK_EQ(brayns::JsonTypeInfo::getType(Poco::makeShared<brayns::JsonObject>()), brayns::JsonType::Object);
    }
    SUBCASE("getType<T>")
    {
        CHECK_EQ(brayns::JsonTypeInfo::getType<brayns::JsonValue>(), brayns::JsonType::Undefined);
        CHECK_EQ(brayns::JsonTypeInfo::getType<brayns::EmptyJson>(), brayns::JsonType::Null);
        CHECK_EQ(brayns::JsonTypeInfo::getType<bool>(), brayns::JsonType::Boolean);
        CHECK_EQ(brayns::JsonTypeInfo::getType<std::int16_t>(), brayns::JsonType::Integer);
        CHECK_EQ(brayns::JsonTypeInfo::getType<std::uint32_t>(), brayns::JsonType::Integer);
        CHECK_EQ(brayns::JsonTypeInfo::getType<float>(), brayns::JsonType::Number);
        CHECK_EQ(brayns::JsonTypeInfo::getType<double>(), brayns::JsonType::Number);
        CHECK_EQ(brayns::JsonTypeInfo::getType<std::string>(), brayns::JsonType::String);
    }
}
