/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/core/utils/StringParser.h>

#include <doctest.h>

using namespace brayns::experimental;

TEST_CASE("Boolean")
{
    CHECK(parseStringAs<bool>("true"));
    CHECK(parseStringAs<bool>("1"));

    CHECK_FALSE(parseStringAs<bool>("false"));
    CHECK_FALSE(parseStringAs<bool>("0"));

    CHECK_THROWS_AS(parseStringAs<bool>(""), std::invalid_argument);
    CHECK_THROWS_AS(parseStringAs<bool>("sggsg"), std::invalid_argument);
}

TEST_CASE("Integer")
{
    CHECK_EQ(parseStringAs<int>("2"), 2);
    CHECK_EQ(parseStringAs<int>("-1"), -1);

    CHECK_THROWS_AS(parseStringAs<std::uint8_t>("256"), std::out_of_range);
    CHECK_THROWS_AS(parseStringAs<std::uint8_t>("-1"), std::out_of_range);

    CHECK_THROWS_AS(parseStringAs<int>("1.5"), std::invalid_argument);
    CHECK_THROWS_AS(parseStringAs<int>(""), std::invalid_argument);
    CHECK_THROWS_AS(parseStringAs<int>("edgsg"), std::invalid_argument);
}

TEST_CASE("Number")
{
    CHECK_EQ(parseStringAs<float>("1.5"), 1.5);
    CHECK_EQ(parseStringAs<float>("-1.5"), -1.5);
    CHECK_EQ(parseStringAs<float>("2"), 2);
    CHECK_EQ(parseStringAs<float>("-1"), -1);

    CHECK_THROWS_AS(parseStringAs<int>(""), std::invalid_argument);
    CHECK_THROWS_AS(parseStringAs<int>("edgsg"), std::invalid_argument);
}
