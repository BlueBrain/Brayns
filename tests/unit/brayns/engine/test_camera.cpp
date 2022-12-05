/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/engine/camera/Camera.h>
#include <brayns/engine/camera/projections/Orthographic.h>
#include <brayns/engine/camera/projections/Perspective.h>
#include <brayns/engine/core/Engine.h>

#include <doctest/doctest.h>

#include <stdexcept>

TEST_CASE("perspective_parameters")
{
    auto params = brayns::ParametersManager(0, nullptr);
    auto engine = brayns::Engine(params);
    (void)engine;

    SUBCASE("Correct parameters constructor")
    {
        auto projection = brayns::Perspective();
        auto camera = brayns::Camera(std::move(projection));
        CHECK_NOTHROW(camera.commit());
    }
    SUBCASE("Incorrect fovy constructor")
    {
        auto projection = brayns::Perspective{0.f};
        auto camera = brayns::Camera(std::move(projection));
        CHECK_THROWS(camera.commit());
    }
    SUBCASE("Correct parameters set()")
    {
        auto camera = brayns::Camera(brayns::Perspective());
        auto projection = brayns::Perspective();
        camera.set(projection);
        CHECK_NOTHROW(camera.commit());
    }
    SUBCASE("Incorrect fovy set()")
    {
        auto camera = brayns::Camera(brayns::Perspective());
        auto projection = brayns::Perspective{-10.f};
        camera.set(projection);
        CHECK_THROWS(camera.commit());
    }
}

TEST_CASE("orthographic_parameters")
{
    auto params = brayns::ParametersManager(0, nullptr);
    auto engine = brayns::Engine(params);
    (void)engine;

    SUBCASE("Correct parameters constructor")
    {
        auto projection = brayns::Orthographic();
        auto camera = brayns::Camera(std::move(projection));
        CHECK_NOTHROW(camera.commit());
    }
    SUBCASE("Incorrect height constructor")
    {
        auto projection = brayns::Orthographic{0.f};
        auto camera = brayns::Camera(std::move(projection));
        CHECK_THROWS_AS(camera.commit(), std::invalid_argument);
    }
    SUBCASE("Correct parameters set()")
    {
        auto camera = brayns::Camera(brayns::Orthographic());
        auto projection = brayns::Orthographic();
        camera.set(projection);
        CHECK_NOTHROW(camera.commit());
    }
    SUBCASE("Incorrect height set()")
    {
        auto camera = brayns::Camera(brayns::Orthographic());
        auto projection = brayns::Orthographic{-10.f};
        camera.set(projection);
        CHECK_THROWS_AS(camera.commit(), std::invalid_argument);
    }
}

TEST_CASE("camera")
{
    auto params = brayns::ParametersManager(0, nullptr);
    auto engine = brayns::Engine(params);
    (void)engine;

    SUBCASE("Projection cast")
    {
        auto camera = brayns::Camera(brayns::Perspective());

        auto goodCast = camera.as<brayns::Perspective>();
        CHECK(goodCast);

        auto badCast = camera.as<brayns::Orthographic>();
        CHECK(!badCast);

        auto badType = camera.as<int>();
        CHECK(!badType);
    }
    SUBCASE("View")
    {
        auto camera = brayns::Camera(brayns::Orthographic());

        auto goodView = brayns::View{brayns::Vector3f(0.f), brayns::Vector3f(1.f), brayns::Vector3f(0.f, 1.f, 0.f)};
        CHECK_NOTHROW(camera.setView(goodView));

        auto badPositionAndTarget = brayns::View{brayns::Vector3f(0.f), brayns::Vector3f(0.f)};
        CHECK_THROWS_AS(camera.setView(badPositionAndTarget), std::invalid_argument);

        auto badUp = brayns::View{brayns::Vector3f(0.f), brayns::Vector3f(1.f), brayns::Vector3f(0.f)};
        CHECK_THROWS_AS(camera.setView(badUp), std::invalid_argument);

        auto viewAndUpAligned =
            brayns::View{brayns::Vector3f(0.f), brayns::Vector3f(0.f, 1.f, 0.f), brayns::Vector3f(0.f, 1.f, 0.f)};
        CHECK_THROWS_AS(camera.setView(viewAndUpAligned), std::invalid_argument);
    }
    SUBCASE("AspectRatio")
    {
        auto camera = brayns::Camera(brayns::Orthographic());

        auto goodFrameSize = brayns::Vector2ui(100, 50);
        CHECK_NOTHROW(camera.setAspectRatioFromFrameSize(goodFrameSize));

        auto badFrameSizeX = brayns::Vector2ui(0, 50);
        CHECK_THROWS_AS(camera.setAspectRatioFromFrameSize(badFrameSizeX), std::invalid_argument);

        auto badFrameSizeY = brayns::Vector2ui(100, 0);
        CHECK_THROWS_AS(camera.setAspectRatioFromFrameSize(badFrameSizeY), std::invalid_argument);
    }
    SUBCASE("Modification")
    {
        auto camera = brayns::Camera(brayns::Perspective());

        camera.set(brayns::Perspective());
        CHECK(camera.commit());

        camera.set(brayns::Orthographic());
        CHECK(camera.commit());
        CHECK(!camera.commit());

        auto view = brayns::View{brayns::Vector3f(3.f)};
        camera.setView(view);
        CHECK(camera.commit());
        camera.setView(view);
        CHECK(!camera.commit());

        auto frameSize = brayns::Vector2ui(100, 50);
        camera.setAspectRatioFromFrameSize(frameSize);
        CHECK(camera.commit());
        camera.setAspectRatioFromFrameSize(frameSize);
        CHECK(!camera.commit());
    }
    SUBCASE("Handle")
    {
        auto camera = brayns::Camera(brayns::Perspective());
        auto &handle = camera.getHandle();
        auto handlePtr = handle.handle();
        CHECK(handlePtr);
    }
}
