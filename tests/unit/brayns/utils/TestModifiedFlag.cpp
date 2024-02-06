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

#include <brayns/utils/ModifiedFlag.h>

#include <doctest/doctest.h>

TEST_CASE("Modified flag")
{
    auto flag = brayns::ModifiedFlag();

    CHECK(flag);
    flag.setModified(false);
    CHECK(!flag);

    int variable = 0;
    CHECK(flag.update(variable, 10));
    CHECK(flag);

    flag.setModified(false);
    CHECK(!flag.update(variable, 10));
    CHECK(!flag);

    flag = true;
    CHECK(flag);
}
