/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/geometry/types/Box.h>
#include <brayns/engine/geometry/types/Sphere.h>
#include <brayns/engine/light/types/QuadLight.h>
#include <brayns/engine/material/types/Metal.h>
#include <brayns/engine/renderer/types/Interactive.h>
#include <brayns/engine/renderer/types/Production.h>

#include <tests/helpers/BraynsTestUtils.h>
#include <tests/helpers/ImageValidator.h>

namespace
{
class CornellBoxBuilder
{
public:
    static void build(BraynsTestUtils &utils)
    {
        _addRoom(utils);
        _addLight(utils);
        _addShapes(utils);
    }

private:
    static void _addRoom(BraynsTestUtils &utils)
    {
        auto floor = brayns::Box{brayns::Vector3f(-5.f, -1.f, -5.f), brayns::Vector3f(5.f, 1.f, 5.f)};
        auto ceiling = brayns::Box{brayns::Vector3f(-5.f, 10.f, -5.f), brayns::Vector3f(5.f, 11.f, 5.f)};
        auto back = brayns::Box{brayns::Vector3f(-5.f, 1.f, -5.f), brayns::Vector3f(5.f, 10.f, -4.5f)};
        auto left = brayns::Box{brayns::Vector3f(-5.f, 1.f, -5.f), brayns::Vector3f(-4.5f, 10.f, 5.f)};
        auto right = brayns::Box{brayns::Vector3f(4.5f, 1.f, -5.f), brayns::Vector3f(5.f, 10.f, 5.f)};

        utils.addGeometry(floor);
        utils.addGeometry(ceiling);
        utils.addGeometry(back);
        utils.addGeometry(left, {}, brayns::Vector4f(0.f, 1.f, 0.f, 1.f));
        utils.addGeometry(right, {}, brayns::Vector4f(1.f, 0.f, 0.f, 1.f));
    }

    static void _addLight(BraynsTestUtils &utils)
    {
        auto pos = brayns::Vector3f(-2.5f, 9.8f, -2.5f);
        auto edge1 = brayns::Vector3f(5.f, 0.f, 0.f);
        auto edge2 = brayns::Vector3f(0.f, 0.f, 5.f);
        auto intensity = 10.f;
        auto color = brayns::Vector3f(1.f);
        auto light = brayns::QuadLight{intensity, color, pos, edge1, edge2};
        utils.addLight(brayns::Light(light));
    }

    static void _addShapes(BraynsTestUtils &utils)
    {
        auto cube1 = brayns::Box{brayns::Vector3f(-1.2f, 1.f, -1.2f), brayns::Vector3f(1.2f, 3.f, 1.2f)};
        auto cube1Translation = brayns::Vector3f(2.f, 0.f, 2.f);
        auto cube1Rotation = brayns::Quaternion(brayns::math::deg2rad(35.f), 0.f, 0.f);
        auto cube1Transform = brayns::Transform{cube1Translation, cube1Rotation};
        utils.addGeometry(cube1, cube1Transform);

        auto cube2 = brayns::Box{brayns::Vector3f(-1.2f, 1.f, -1.2f), brayns::Vector3f(1.2f, 5.f, 1.2f)};
        auto cube2Translation = brayns::Vector3f(-2.f, 0.f, -2.f);
        auto cube2Rotation = brayns::Quaternion(brayns::math::deg2rad(-35.f), 0.f, 0.f);
        auto cube2Transform = brayns::Transform{cube2Translation, cube2Rotation};
        utils.addGeometry(cube2, cube2Transform);

        auto sphere = brayns::Sphere{brayns::Vector3f(-2.f, 6.f, -2.f), 1.f};
        auto sphereInstance = utils.addGeometry(sphere);
        auto &sphereModel = sphereInstance->getModel();
        auto &sphereComponents = sphereModel.getComponents();
        sphereComponents.add<brayns::Material>(brayns::Metal());
    }
};
}

TEST_CASE("Renderer change")
{
    auto utils = BraynsTestUtils();

    CornellBoxBuilder::build(utils);
    utils.adjustPerspectiveView();

    CHECK(ImageValidator::validate(utils.render(), "test_renderer_default.png"));

    utils.setRenderer(brayns::Production{64, 3});
    CHECK(ImageValidator::validate(utils.render(), "test_renderer_switch.png"));

    utils.setRenderer(brayns::Interactive());
    CHECK(ImageValidator::validate(utils.render(), "test_renderer_switch_back_default.png"));
}

TEST_CASE("Renderer type")
{
    SUBCASE("Interactive")
    {
        auto utils = BraynsTestUtils();
        CornellBoxBuilder::build(utils);
        utils.adjustPerspectiveView();
        utils.setRenderer(brayns::Interactive{16, 3});
        CHECK(ImageValidator::validate(utils.render(), "test_renderer_interactive.png"));
    }
    SUBCASE("Production")
    {
        auto utils = BraynsTestUtils();
        CornellBoxBuilder::build(utils);
        utils.adjustPerspectiveView();
        utils.setRenderer(brayns::Production{64, 3});
        CHECK(ImageValidator::validate(utils.render(), "test_renderer_production.png"));
    }
}
