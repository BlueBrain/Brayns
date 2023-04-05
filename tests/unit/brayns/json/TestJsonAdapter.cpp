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

#include <brayns/json/Json.h>
#include <brayns/json/adapters/ArrayAdapter.h>
#include <brayns/json/adapters/EnumAdapter.h>
#include <brayns/json/adapters/GlmAdapter.h>
#include <brayns/json/adapters/MapAdapter.h>
#include <brayns/json/adapters/PrimitiveAdapter.h>
#include <brayns/json/adapters/PtrAdapter.h>

enum class TestEnum
{
    Test1,
    Test2,
};

namespace brayns
{
template<>
struct EnumReflector<TestEnum>
{
    static EnumMap<TestEnum> reflect()
    {
        return {{"test1", TestEnum::Test1}, {"test2", TestEnum::Test2}};
    }
};

template<>
struct JsonAdapter<TestEnum> : EnumAdapter<TestEnum>
{
};
} // namespace brayns

TEST_CASE("JsonAdapters")
{
    SUBCASE("Boolean")
    {
        auto schema = brayns::Json::getSchema<bool>();
        CHECK_EQ(schema.type, brayns::JsonType::Boolean);
    }
    SUBCASE("Integer")
    {
        auto schema = brayns::Json::getSchema<int>();
        CHECK_EQ(schema.type, brayns::JsonType::Integer);
    }
    SUBCASE("Number")
    {
        auto schema = brayns::Json::getSchema<float>();
        CHECK_EQ(schema.type, brayns::JsonType::Number);
    }
    SUBCASE("String")
    {
        auto schema = brayns::Json::getSchema<std::string>();
        CHECK_EQ(schema.type, brayns::JsonType::String);
    }
    SUBCASE("Array")
    {
        auto schema = brayns::Json::getSchema<std::vector<int>>();
        CHECK_EQ(schema.type, brayns::JsonType::Array);
        CHECK_EQ(schema.items.size(), 1);
        CHECK_EQ(schema.items[0].type, brayns::JsonType::Integer);
        auto json = brayns::Json::stringify(std::vector<int>({1, 2, 3}));
        CHECK_EQ(json, "[1,2,3]");
        auto value = brayns::Json::parse<std::vector<int>>("[1, 2, 3]");
        CHECK_EQ(value, std::vector<int>({1, 2, 3}));
    }
    SUBCASE("Hash")
    {
        auto schema = brayns::Json::getSchema<std::map<std::string, int>>();
        CHECK_EQ(schema.type, brayns::JsonType::Object);
        CHECK_EQ(schema.items.size(), 1);
        CHECK_EQ(schema.items[0].type, brayns::JsonType::Integer);
        auto json = brayns::Json::stringify(std::map<std::string, int>({{"test", 1}}));
        CHECK_EQ(json, "{\"test\":1}");
        auto value = brayns::Json::parse<std::map<std::string, int>>("{\"test\": 1}");
        CHECK_EQ(value, std::map<std::string, int>({{"test", 1}}));
    }
    SUBCASE("Enum")
    {
        auto schema = brayns::Json::getSchema<TestEnum>();
        CHECK_EQ(schema.type, brayns::JsonType::String);
        CHECK_EQ(schema.enums.size(), 2);
        CHECK_EQ(schema.enums[0], "test1");
        CHECK_EQ(schema.enums[1], "test2");
    }
    SUBCASE("Ptr")
    {
        auto schema = brayns::Json::getSchema<std::unique_ptr<int>>();
        CHECK_EQ(schema.type, brayns::JsonType::Integer);
    }
}
