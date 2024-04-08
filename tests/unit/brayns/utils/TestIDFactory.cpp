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

#include <brayns/core/utils/IDFactory.h>

#include <doctest/doctest.h>

TEST_CASE("ID factory")
{
    auto factory = brayns::IDFactory<uint8_t>();

    CHECK(factory.generateID() == 0);
    CHECK(factory.generateID() == 1);

    factory.releaseID(0);
    CHECK(factory.generateID() == 0);
    CHECK(factory.generateID() == 2);

    factory.releaseID(2);
    factory.clear();
    CHECK(factory.generateID() == 0);
    CHECK(factory.generateID() == 1);

    factory.clear();

    for (uint8_t i = 0; i < std::numeric_limits<uint8_t>::max(); ++i)
    {
        CHECK(factory.generateID() == i);
    }
    CHECK_THROWS_WITH(factory.generateID(), "ID factory exhausted");
}
