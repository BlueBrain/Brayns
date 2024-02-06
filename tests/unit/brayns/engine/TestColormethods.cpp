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

#include <brayns/engine/colormethods/PrimitiveColorMethod.h>
#include <brayns/engine/colormethods/SolidColorMethod.h>
#include <brayns/engine/components/ColorList.h>
#include <brayns/engine/components/ColorSolid.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/geometry/types/Sphere.h>
#include <brayns/engine/model/Components.h>

#include <tests/unit/PlaceholderEngine.h>

#include <doctest/doctest.h>

TEST_CASE("Primitive color method")
{
    SUBCASE("Constructor")
    {
        auto colorMethod = brayns::PrimitiveColorMethod("test", 10);
        auto components = brayns::Components();
        CHECK(colorMethod.getName() == "test");
        CHECK(colorMethod.getValues(components) == std::vector<std::string>{"0-9"});
    }
    SUBCASE("Apply")
    {
        BRAYNS_TESTS_PLACEHOLDER_ENGINE

        auto colorMethod = brayns::PrimitiveColorMethod("test", 10);

        auto components = brayns::Components();
        auto &geometries = components.add<brayns::Geometries>(brayns::Sphere());
        auto &views = components.add<brayns::GeometryViews>(geometries.elements);
        CHECK(!components.has<brayns::ColorList>());
        CHECK_NOTHROW(colorMethod.apply(components, {{"0", brayns::Vector4f(1.f)}}));
        CHECK(components.has<brayns::ColorList>());
        CHECK(views.modified);

        CHECK_THROWS_AS(colorMethod.apply(components, {{"8-10", brayns::Vector4f(1.f)}}), std::range_error);
        CHECK_THROWS_AS(colorMethod.apply(components, {{"10", brayns::Vector4f(1.f)}}), std::range_error);
    }
}

TEST_CASE("Solid color method")
{
    BRAYNS_TESTS_PLACEHOLDER_ENGINE

    auto colorMethod = brayns::SolidColorMethod();

    auto components = brayns::Components();
    auto &geometries = components.add<brayns::Geometries>(brayns::Sphere());
    auto &views = components.add<brayns::GeometryViews>(geometries.elements);
    CHECK(!components.has<brayns::ColorSolid>());
    CHECK_NOTHROW(colorMethod.apply(components, {{"color", brayns::Vector4f(1.f)}}));
    CHECK(components.has<brayns::ColorSolid>());
    CHECK(views.modified);

    CHECK_THROWS_AS(colorMethod.apply(components, {{"key", brayns::Vector4f(1.f)}}), std::invalid_argument);
}
