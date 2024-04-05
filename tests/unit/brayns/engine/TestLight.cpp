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

#include <brayns/core/engine/light/Light.h>
#include <brayns/core/engine/light/types/AmbientLight.h>
#include <brayns/core/engine/light/types/DirectionalLight.h>
#include <brayns/core/engine/light/types/QuadLight.h>

#include <unit/PlaceholderEngine.h>

TEST_CASE("Light Casting")
{
    BRAYNS_TESTS_PLACEHOLDER_ENGINE

    auto light = brayns::Light(brayns::AmbientLight());
    CHECK(light.as<brayns::AmbientLight>());
    CHECK(!light.as<brayns::DirectionalLight>());
}

TEST_CASE("Light bounds")
{
    BRAYNS_TESTS_PLACEHOLDER_ENGINE

    auto noTransform = brayns::TransformMatrix();
    auto transform = brayns::Transform{.translation = brayns::Vector3f(100.f, 0.f, 0.f)}.toMatrix();

    auto emptyBounds = brayns::Bounds();

    SUBCASE("Ambient light")
    {
        auto ambientLight = brayns::Light(brayns::AmbientLight());
        auto ambientBounds = ambientLight.computeBounds(noTransform);
        CHECK(ambientBounds.getMax() == emptyBounds.getMax());
        CHECK(ambientBounds.getMin() == emptyBounds.getMin());
        ambientBounds = ambientLight.computeBounds(transform);
        CHECK(ambientBounds.getMax() == emptyBounds.getMax());
        CHECK(ambientBounds.getMin() == emptyBounds.getMin());
    }
    SUBCASE("Directional light")
    {
        auto directionalLight = brayns::Light(brayns::DirectionalLight());
        auto directionalBounds = directionalLight.computeBounds(noTransform);
        CHECK(directionalBounds.getMax() == emptyBounds.getMax());
        CHECK(directionalBounds.getMin() == emptyBounds.getMin());
        directionalBounds = directionalLight.computeBounds(transform);
        CHECK(directionalBounds.getMax() == emptyBounds.getMax());
        CHECK(directionalBounds.getMin() == emptyBounds.getMin());
    }
    SUBCASE("Quad light")
    {
        auto quadLight = brayns::Light(brayns::QuadLight());
        auto quadBounds = quadLight.computeBounds(noTransform);
        CHECK(quadBounds.getMin() == brayns::Vector3f(0.f));
        CHECK(quadBounds.getMax() == brayns::Vector3f(1.f, 0.f, 1.f));
        quadBounds = quadLight.computeBounds(transform);
        CHECK(quadBounds.getMin() == brayns::Vector3f(100.f, 0.f, 0.f));
        CHECK(quadBounds.getMax() == brayns::Vector3f(101.f, 0.f, 1.f));
    }
}
