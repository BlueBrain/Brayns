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

#define BOOST_TEST_MODULE circuitViewerUtils
#include <boost/test/unit_test.hpp>

using GIDSet = brion::GIDSet;
GIDSet keyToGIDorRange(std::string s, const double fraction = 1.f,
                       const int32_t seed = 0)
{
    return brayns::_keyToGIDorRange(s, fraction, seed);
}

BOOST_AUTO_TEST_CASE(gids)
{
    BOOST_CHECK(keyToGIDorRange("1") == GIDSet{1});
}

BOOST_AUTO_TEST_CASE(gid_ranges)
{
    BOOST_CHECK(keyToGIDorRange("1-1") == GIDSet{1});
    BOOST_CHECK(keyToGIDorRange("1-2") == GIDSet({1, 2}));
    BOOST_CHECK_EQUAL(keyToGIDorRange("1-10", 0.5, 0).size(), 5);
    BOOST_CHECK_THROW(keyToGIDorRange("1-0"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(invalid)
{
    BOOST_CHECK(keyToGIDorRange("1a").empty());
    BOOST_CHECK(keyToGIDorRange("1 a").empty());
    BOOST_CHECK(keyToGIDorRange("1a").empty());
    BOOST_CHECK(keyToGIDorRange("1-2a").empty());
    BOOST_CHECK(keyToGIDorRange("1 1-2").empty());
}


