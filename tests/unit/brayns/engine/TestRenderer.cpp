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

#include <brayns/engine/renderer/Renderer.h>
#include <brayns/engine/renderer/types/Interactive.h>
#include <brayns/engine/renderer/types/Production.h>

#include <tests/unit/PlaceholderEngine.h>

TEST_CASE("Renderer")
{
    BRAYNS_TESTS_PLACEHOLDER_ENGINE

    SUBCASE("Interactive constructor")
    {
        CHECK_NOTHROW(brayns::Renderer(brayns::Interactive()));

        auto badAoSamples = brayns::Interactive();
        badAoSamples.aoSamples = static_cast<size_t>(std::numeric_limits<int>::max()) + 1;
        CHECK_THROWS_AS(brayns::Renderer(std::move(badAoSamples)), std::invalid_argument);

        auto badBounces = brayns::Interactive();
        badBounces.maxRayBounces = static_cast<size_t>(std::numeric_limits<int>::max()) + 1;
        CHECK_THROWS_AS(brayns::Renderer(std::move(badBounces)), std::invalid_argument);

        auto badSamplesPerPixel = brayns::Interactive();
        badSamplesPerPixel.samplesPerPixel = static_cast<size_t>(std::numeric_limits<int>::max()) + 1;
        CHECK_THROWS_AS(brayns::Renderer(std::move(badSamplesPerPixel)), std::invalid_argument);
    }
    SUBCASE("Interactive set")
    {
        auto renderer = brayns::Renderer(brayns::Interactive());

        CHECK_NOTHROW(renderer.set(brayns::Interactive()));

        auto badAoSamples = brayns::Interactive();
        badAoSamples.aoSamples = static_cast<size_t>(std::numeric_limits<int>::max()) + 1;
        CHECK_THROWS_AS(renderer.set(badAoSamples), std::invalid_argument);

        auto badBounces = brayns::Interactive();
        badBounces.maxRayBounces = static_cast<size_t>(std::numeric_limits<int>::max()) + 1;
        CHECK_THROWS_AS(renderer.set(badBounces), std::invalid_argument);

        auto badSamplesPerPixel = brayns::Interactive();
        badSamplesPerPixel.samplesPerPixel = static_cast<size_t>(std::numeric_limits<int>::max()) + 1;
        CHECK_THROWS_AS(renderer.set(badSamplesPerPixel), std::invalid_argument);
    }
    SUBCASE("Production constructor")
    {
        CHECK_NOTHROW(brayns::Renderer(brayns::Production()));

        auto badBounces = brayns::Production();
        badBounces.maxRayBounces = static_cast<size_t>(std::numeric_limits<int>::max()) + 1;
        CHECK_THROWS_AS(brayns::Renderer(std::move(badBounces)), std::invalid_argument);

        auto badSamplesPerPixel = brayns::Production();
        badSamplesPerPixel.samplesPerPixel = static_cast<size_t>(std::numeric_limits<int>::max()) + 1;
        CHECK_THROWS_AS(brayns::Renderer(std::move(badSamplesPerPixel)), std::invalid_argument);
    }
    SUBCASE("Production set")
    {
        auto renderer = brayns::Renderer(brayns::Production());

        CHECK_NOTHROW(renderer.set(brayns::Production()));

        auto badBounces = brayns::Production();
        badBounces.maxRayBounces = static_cast<size_t>(std::numeric_limits<int>::max()) + 1;
        CHECK_THROWS_AS(renderer.set(badBounces), std::invalid_argument);

        auto badSamplesPerPixel = brayns::Production();
        badSamplesPerPixel.samplesPerPixel = static_cast<size_t>(std::numeric_limits<int>::max()) + 1;
        CHECK_THROWS_AS(renderer.set(badSamplesPerPixel), std::invalid_argument);
    }
    SUBCASE("Commit")
    {
        auto renderer = brayns::Renderer(brayns::Production());
        CHECK(renderer.commit());
        CHECK(!renderer.commit());
    }
    SUBCASE("Samples per pixel")
    {
        auto interactive = brayns::Interactive();
        interactive.samplesPerPixel = 456;
        auto renderer = brayns::Renderer(interactive);
        CHECK(renderer.getSamplesPerPixel() == 456);
    }
}
