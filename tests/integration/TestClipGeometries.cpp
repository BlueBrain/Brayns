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

#include <brayns/engine/geometry/types/BoundedPlane.h>
#include <brayns/engine/geometry/types/Box.h>
#include <brayns/engine/geometry/types/Capsule.h>
#include <brayns/engine/geometry/types/Plane.h>
#include <brayns/engine/geometry/types/Sphere.h>
#include <brayns/engine/geometry/types/TriangleMesh.h>
#include <brayns/engine/renderer/types/Interactive.h>
#include <brayns/engine/systems/ClipperInitSystem.h>
#include <brayns/io/loaders/mesh/parsers/ObjMeshParser.h>
#include <brayns/utils/FileReader.h>

#include <tests/helpers/BraynsTestUtils.h>
#include <tests/helpers/ImageValidator.h>
#include <tests/paths.h>

#include <doctest/doctest.h>

namespace
{
class ClipGeometryBuilder
{
public:
    template<typename T>
    static void build(brayns::Brayns &brayns, T geometry, const brayns::Transform transform = {})
    {
        auto model = std::make_shared<brayns::Model>("clip_geometry");

        auto &components = model->getComponents();
        components.add<brayns::Geometries>(std::move(geometry));

        auto &systems = model->getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        systems.setInitSystem<brayns::ClipperInitSystem>();

        auto &engine = brayns.getEngine();
        auto &scene = engine.getScene();
        auto &models = scene.getModels();
        auto instance = models.add(std::move(model));
        instance->setTransform(transform);
    }
};

class ClipGeometryTypeTester
{
public:
    template<typename T>
    static void testType(T geometry, const std::string &filename, const brayns::Transform &transform = {})
    {
        auto args = "brayns";
        auto brayns = brayns::Brayns(1, &args);

        auto utils = BraynsTestUtils(brayns);
        utils.addDefaultLights();
        utils.addGeometry(brayns::Sphere{brayns::Vector3f(0.f), 10.f});
        utils.adjustPerspectiveView();

        ClipGeometryBuilder::build(brayns, geometry, transform);

        auto interactive = brayns::Interactive();
        interactive.shadowsEnabled = false;
        utils.setRenderer(interactive);

        CHECK(ImageValidator::validate(utils.render(), filename));
    }
};
}

TEST_CASE("Clip geometry add/removal")
{
    auto args = "brayns";
    auto brayns = brayns::Brayns(1, &args);

    auto utils = BraynsTestUtils(brayns);

    utils.addDefaultLights();
    utils.addGeometry(brayns::Sphere{brayns::Vector3f(0.f), 10.f}, {});
    utils.adjustPerspectiveView();

    ClipGeometryBuilder::build(brayns, brayns::Plane{{1.f, 0.f, 0.f, 0.f}});

    CHECK(ImageValidator::validate(utils.render(), "test_clip_geometry_add.png"));

    utils.removeClipping();

    CHECK(ImageValidator::validate(utils.render(), "test_clip_geometry_remove.png"));

    ClipGeometryBuilder::build(brayns, brayns::Plane{{-1.f, 0.f, 0.f, 0.f}});

    CHECK(ImageValidator::validate(utils.render(), "test_clip_geometry_re_add.png"));
}

TEST_CASE("Clip geometry types")
{
    SUBCASE("Bounded plane")
    {
        auto min = brayns::Vector3f(0.f);
        auto max = brayns::Vector3f(11.f);
        auto equation = brayns::Vector4f{0.f, 0.f, 1.f, 0.f};
        ClipGeometryTypeTester::testType(
            brayns::BoundedPlane{equation, {min, max}},
            "test_clip_geometry_bounded_plane.png");
    }
    SUBCASE("Box")
    {
        auto min = brayns::Vector3f(-2.5f, -2.5f, -11.f);
        auto max = brayns::Vector3f(2.5f, 2.5f, 11.f);
        auto transform = brayns::Transform();
        transform.rotation = glm::quat_cast(glm::rotate(glm::radians(-45.f), brayns::Vector3f(0.f, 1.f, 0.f)));
        ClipGeometryTypeTester::testType(brayns::Box{min, max}, "test_clip_geometry_box.png", transform);
    }
    SUBCASE("Capsule")
    {
        auto p0 = brayns::Vector3f(0.f, -10.f, 7.5f);
        auto r0 = 6.f;
        auto p1 = brayns::Vector3f(0.f, 10.f, 7.5f);
        auto r1 = 3.f;
        ClipGeometryTypeTester::testType(brayns::Capsule{p0, r0, p1, r1}, "test_clip_geometry_capsule.png");
    }
    SUBCASE("Plane")
    {
        auto equation = brayns::Vector4f(0.f, 0.f, 1.f, 0.f);
        auto transform = brayns::Transform();
        transform.rotation = glm::quat_cast(glm::rotate(glm::radians(-45.f), brayns::Vector3f(0.f, 1.f, 0.f)));
        ClipGeometryTypeTester::testType(brayns::Plane{equation}, "test_clip_geometry_plane.png", transform);
    }
    SUBCASE("Sphere")
    {
        auto center = brayns::Vector3f(0.f, 0.f, 6.f);
        auto radius = 7.f;
        ClipGeometryTypeTester::testType(brayns::Sphere{center, radius}, "test_clip_geometry_sphere.png");
    }
    SUBCASE("Triangle mesh")
    {
        auto content = brayns::FileReader::read(TestPaths::Meshes::suzanne);
        auto parser = brayns::ObjMeshParser();
        auto mesh = parser.parse(content);
        auto bounds = brayns::GeometryTraits<brayns::TriangleMesh>::computeBounds({}, mesh);

        auto dimensions = bounds.dimensions();
        auto scale = brayns::Vector3f(5.f);
        auto position = -bounds.center();
        position.z += 10.f - dimensions.z * 0.5f;
        auto transform = brayns::Transform{position, {}, scale};

        ClipGeometryTypeTester::testType(mesh, "test_clip_geometry_mesh.png", transform);
    }
}
