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

#include <brayns/core/engine/components/Bounds.h>

TEST_CASE("Bounds")
{
    SUBCASE("Default constructor")
    {
        auto bounds = brayns::Bounds();
        auto &min = bounds.getMin();
        auto &max = bounds.getMax();

        CHECK(min.x == doctest::Approx(std::numeric_limits<float>::max()));
        CHECK(min.y == doctest::Approx(std::numeric_limits<float>::max()));
        CHECK(min.z == doctest::Approx(std::numeric_limits<float>::max()));

        CHECK(max.x == doctest::Approx(std::numeric_limits<float>::lowest()));
        CHECK(max.y == doctest::Approx(std::numeric_limits<float>::lowest()));
        CHECK(max.z == doctest::Approx(std::numeric_limits<float>::lowest()));
    }
    SUBCASE("Constructor")
    {
        auto min = brayns::Vector3f(1.f, 2.f, 3.f);
        auto max = brayns::Vector3f(4.f, 5.f, 6.f);
        CHECK_NOTHROW(brayns::Bounds(min, max));

        auto bounds = brayns::Bounds(min, max);
        CHECK(bounds.getMin() == min);
        CHECK(bounds.getMax() == max);
        CHECK(bounds.center() == (min + max) * 0.5f);
        CHECK(bounds.dimensions() == (max - min));

        auto badMin = max;
        auto badMax = min;
        CHECK_THROWS_AS(brayns::Bounds(badMin, badMax), std::invalid_argument);
    }
    SUBCASE("Expand point")
    {
        auto min = brayns::Vector3f(0.f);
        auto max = brayns::Vector3f(10.f);
        auto bounds = brayns::Bounds(min, max);

        auto p1 = brayns::Vector3f(5.f);
        bounds.expand(p1);
        CHECK(bounds.getMin() == min);
        CHECK(bounds.getMax() == max);

        auto p2 = brayns::Vector3f(-1.f);
        bounds.expand(p2);
        CHECK(bounds.getMin() == p2);
        CHECK(bounds.getMax() == max);

        auto p3 = brayns::Vector3f(5.f, 50.f, 5.f);
        bounds.expand(p3);
        CHECK(bounds.getMin() == p2);
        CHECK(bounds.getMax() == brayns::Vector3f(10.f, 50.f, 10.f));
    }
    SUBCASE("Expand bounds")
    {
        auto min = brayns::Vector3f(0.f);
        auto max = brayns::Vector3f(10.f);
        auto bounds = brayns::Bounds(min, max);

        {
            auto bMin = brayns::Vector3f(1.f);
            auto bMax = brayns::Vector3f(9.f);
            auto testBounds = brayns::Bounds(bMin, bMax);
            bounds.expand(testBounds);
            CHECK(bounds.getMin() == min);
            CHECK(bounds.getMax() == max);
        }
        {
            auto bMin = brayns::Vector3f(-1.f, 2.f, 2.f);
            auto bMax = brayns::Vector3f(9.f);
            auto testBounds = brayns::Bounds(bMin, bMax);
            bounds.expand(testBounds);
            CHECK(bounds.getMin() == brayns::Vector3f(-1.f, 0.f, 0.f));
            CHECK(bounds.getMax() == max);
        }
        {
            auto bMin = brayns::Vector3f(2.f);
            auto bMax = brayns::Vector3f(20.f);
            auto testBounds = brayns::Bounds(bMin, bMax);
            bounds.expand(testBounds);
            CHECK(bounds.getMin() == brayns::Vector3f(-1.f, 0.f, 0.f));
            CHECK(bounds.getMax() == brayns::Vector3f(20.f));
        }
        {
            auto bMin = brayns::Vector3f(-30.f);
            auto bMax = brayns::Vector3f(30.f);
            auto testBounds = brayns::Bounds(bMin, bMax);
            bounds.expand(testBounds);
            CHECK(bounds.getMin() == bMin);
            CHECK(bounds.getMax() == bMax);
        }
    }
    SUBCASE("Intersect point")
    {
        auto min = brayns::Vector3f(0.f);
        auto max = brayns::Vector3f(10.f);
        const auto bounds = brayns::Bounds(min, max);

        CHECK(bounds.intersects(brayns::Vector3f(5.f)));
        CHECK(!bounds.intersects(brayns::Vector3f(-1.f, 5.f, 5.f)));
    }
    SUBCASE("Intersect bounds")
    {
        auto min = brayns::Vector3f(0.f);
        auto max = brayns::Vector3f(10.f);
        const auto bounds = brayns::Bounds(min, max);

        {
            auto bMin = brayns::Vector3f(1.f);
            auto bMax = brayns::Vector3f(9.f);
            auto testBounds = brayns::Bounds(bMin, bMax);
            CHECK(bounds.intersects(testBounds));
        }
        {
            auto bMin = brayns::Vector3f(-1.f);
            auto bMax = brayns::Vector3f(1.f);
            auto testBounds = brayns::Bounds(bMin, bMax);
            CHECK(bounds.intersects(testBounds));
        }
        {
            auto bMin = brayns::Vector3f(-20.f);
            auto bMax = brayns::Vector3f(-10.f);
            auto testBounds = brayns::Bounds(bMin, bMax);
            CHECK(!bounds.intersects(testBounds));
        }
        {
            auto bMin = brayns::Vector3f(-30.f);
            auto bMax = brayns::Vector3f(30.f);
            auto testBounds = brayns::Bounds(bMin, bMax);
            CHECK(bounds.intersects(testBounds));
        }
    }
}
