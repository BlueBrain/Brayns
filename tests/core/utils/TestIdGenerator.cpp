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

#include <brayns/core/utils/IdGenerator.h>

#include <doctest.h>

using namespace brayns;

TEST_CASE("ID generation")
{
    auto ids = IdGenerator<std::int32_t>();

    CHECK_EQ(ids.next(), 0);
    CHECK_EQ(ids.next(), 1);
    CHECK_EQ(ids.next(), 2);

    ids.recycle(1);
    CHECK_EQ(ids.next(), 1);

    ids.reset();

    CHECK_EQ(ids.next(), 0);
}

TEST_CASE("Limits")
{
    auto ids = IdGenerator<std::uint32_t>(2, 4);

    CHECK_EQ(ids.next(), 2);
    CHECK_EQ(ids.next(), 3);

    CHECK_THROWS_AS(ids.next(), std::out_of_range);

    ids.recycle(2);
    CHECK_EQ(ids.next(), 2);
}
