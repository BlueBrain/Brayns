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

#include "TestCase.h"

#include <brayns/core/engine/geometry/Geometry.h>
#include <brayns/core/engine/geometry/types/Box.h>
#include <brayns/core/engine/geometry/types/Sphere.h>

#include "PlaceholderEngine.h"

TEST_CASE("Geometry")
{
    BRAYNS_TESTS_PLACEHOLDER_ENGINE

    SUBCASE("Constructor")
    {
        auto sphere = brayns::Sphere();
        auto geometry = brayns::Geometry(sphere);
        CHECK(geometry.numPrimitives() == 1);

        auto spheres = std::vector<brayns::Sphere>{
            brayns::Sphere(),
            brayns::Sphere(),
            brayns::Sphere(),
        };
        geometry = brayns::Geometry(std::move(spheres));
        CHECK(geometry.numPrimitives() == 3);
    }
    SUBCASE("Casting")
    {
        auto geometry = brayns::Geometry(brayns::Sphere());
        CHECK(geometry.as<brayns::Sphere>());
        CHECK(!geometry.as<brayns::Box>());
    }
    SUBCASE("Iteration")
    {
        auto spheres = std::vector<brayns::Sphere>{
            brayns::Sphere(),
            brayns::Sphere(),
            brayns::Sphere(),
        };
        auto geometry = brayns::Geometry(std::move(spheres));
        geometry.commit();

        CHECK(!geometry.commit());

        auto counter = 0ul;
        geometry.forEach(
            [&](brayns::Sphere &sphere)
            {
                (void)sphere;
                ++counter;
            });
        CHECK(counter == 3);
        CHECK(geometry.commit());
    }
    SUBCASE("Compute bounds")
    {
        auto sphere = brayns::Sphere{brayns::Vector3f(0.f), 10.f};
        auto geometry = brayns::Geometry(sphere);

        auto bounds = geometry.computeBounds(brayns::TransformMatrix());
        auto min = bounds.getMin();
        auto max = bounds.getMax();
        CHECK(min == brayns::Vector3f(-10.f));
        CHECK(max == brayns::Vector3f(10.f));

        auto transform = brayns::Transform{.translation = brayns::Vector3f(100.f, 0.f, 0.f)}.toMatrix();
        bounds = geometry.computeBounds(transform);
        min = bounds.getMin();
        max = bounds.getMax();
        CHECK(min == brayns::Vector3f(90.f, -10.f, -10.f));
        CHECK(max == brayns::Vector3f(110.f, 10.f, 10.f));
    }
}
