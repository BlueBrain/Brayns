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

#include <brayns/engine/model/Components.h>

TEST_CASE("Components")
{
    SUBCASE("Add component")
    {
        auto components = brayns::Components();
        CHECK(!components.has<int>());

        components.add<int>(58);
        CHECK(components.has<int>());
        CHECK(components.get<int>() == 58);

        components.add<int>(78);
        CHECK(components.get<int>() == 78);
    }
    SUBCASE("Get or add component")
    {
        auto components = brayns::Components();
        CHECK(!components.has<int>());

        components.add<int>(58);
        CHECK(components.has<int>());
        CHECK(components.get<int>() == 58);

        components.getOrAdd<int>(78);
        CHECK(components.get<int>() == 58);
    }
    SUBCASE("Find")
    {
        auto components = brayns::Components();
        CHECK(!components.has<int>());

        components.add<int>(58);
        CHECK(components.find<int>());

        CHECK(!components.find<float>());
    }
}
