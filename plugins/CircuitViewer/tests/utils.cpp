/* Copyright (c) 2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Juan.Hernando@epfl.ch
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

#include "../io/utils.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "tests/doctest.h"

using GIDSet = brion::GIDSet;
GIDSet keyToGIDorRange(std::string s, const double fraction = 1.f,
                       const int32_t seed = 0)
{
    return brayns::_keyToGIDorRange(s, fraction, seed);
}

TEST_CASE("gids")
{
    CHECK_EQ(keyToGIDorRange("1"), GIDSet{1});
}

TEST_CASE("gid_ranges")
{
    CHECK_EQ(keyToGIDorRange("1-1"), GIDSet{1});
    CHECK_EQ(keyToGIDorRange("1-2"), GIDSet({1, 2}));
    CHECK_EQ(keyToGIDorRange("1-10", 0.5, 0).size(), 5);
    CHECK_THROWS(keyToGIDorRange("1-0"));
}

TEST_CASE("invalid")
{
    CHECK(keyToGIDorRange("1a").empty());
    CHECK(keyToGIDorRange("1 a").empty());
    CHECK(keyToGIDorRange("1a").empty());
    CHECK(keyToGIDorRange("1-2a").empty());
    CHECK(keyToGIDorRange("1 1-2").empty());
}
