/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/engine/common/ClippingModelBuilder.h>
#include <brayns/engine/geometry/types/BoundedPlane.h>
#include <brayns/engine/geometry/types/Box.h>
#include <brayns/engine/geometry/types/Capsule.h>
#include <brayns/engine/geometry/types/Plane.h>
#include <brayns/engine/geometry/types/Sphere.h>
#include <brayns/engine/geometry/types/TriangleMesh.h>
#include <brayns/engine/renderer/types/Interactive.h>
#include <brayns/io/loaders/mesh/parsers/ObjMeshParser.h>
#include <brayns/utils/FileReader.h>

#include <tests/helpers/BraynsTestUtils.h>
#include <tests/helpers/ImageValidator.h>
#include <tests/paths.h>

#include <spdlog/fmt/fmt.h>

#include <doctest/doctest.h>

namespace
{
class ClippingBuilder
{
public:
    template<typename T>
    static void build(
        brayns::Engine &engine,
        T geometry,
        const brayns::Transform transform = {},
        bool invertNormals = false)
    {
        auto model = brayns::ClippingModelBuilder::build<T>({std::move(geometry)}, invertNormals);

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
    static void testType(
        T geometry,
        const std::string &filename,
        const brayns::Transform &transform = {},
        bool invertNormals = false)
    {
        auto utils = BraynsTestUtils();
        utils.addDefaultLights();
        utils.addGeometry(brayns::Sphere{brayns::Vector3f(0.f), 10.f});
        utils.adjustPerspectiveView();

        ClippingBuilder::build(utils.getEngine(), geometry, transform, invertNormals);

        auto interactive = brayns::Interactive();
        interactive.shadowsEnabled = false;
        utils.setRenderer(interactive);

        CHECK(ImageValidator::validate(utils.render(), filename));
    }
};

class ClipGeometryTester
{
public:
    static void test(bool invertNormals)
    {
        SUBCASE("Bounded plane")
        {
            auto min = brayns::Vector3f(0.f);
            auto max = brayns::Vector3f(11.f);
            auto equation = brayns::Vector4f{0.f, 0.f, 1.f, 0.f};
            auto plane = brayns::BoundedPlane{equation, {min, max}};
            auto reference = _filename("test_clip_geometry_bounded_plane", invertNormals);
            ClipGeometryTypeTester::testType(plane, reference, {}, invertNormals);
        }
        SUBCASE("Box")
        {
            auto min = brayns::Vector3f(-2.5f, -2.5f, -11.f);
            auto max = brayns::Vector3f(2.5f, 2.5f, 11.f);
            auto box = brayns::Box{min, max};
            auto transform = brayns::Transform();
            transform.rotation = brayns::Quaternion(brayns::math::deg2rad(-45.f), 0.f, 0.f);
            auto reference = _filename("test_clip_geometry_box", invertNormals);
            ClipGeometryTypeTester::testType(box, reference, transform, invertNormals);
        }
        SUBCASE("Capsule")
        {
            auto p0 = brayns::Vector3f(0.f, -10.f, 7.5f);
            auto r0 = 6.f;
            auto p1 = brayns::Vector3f(0.f, 10.f, 7.5f);
            auto r1 = 3.f;
            auto capsule = brayns::Capsule{p0, r0, p1, r1};
            auto reference = _filename("test_clip_geometry_capsule", invertNormals);
            ClipGeometryTypeTester::testType(capsule, reference, {}, invertNormals);
        }
        SUBCASE("Plane")
        {
            auto equation = brayns::Vector4f(0.f, 0.f, 1.f, 0.f);
            auto transform = brayns::Transform();
            transform.rotation = brayns::Quaternion(brayns::math::deg2rad(-45.f), 0.f, 0.f);
            auto plane = brayns::Plane{equation};
            auto reference = _filename("test_clip_geometry_plane", invertNormals);
            ClipGeometryTypeTester::testType(plane, reference, transform, invertNormals);
        }
        SUBCASE("Sphere")
        {
            auto center = brayns::Vector3f(0.f, 0.f, 6.f);
            auto radius = 7.f;
            auto sphere = brayns::Sphere{center, radius};
            auto reference = _filename("test_clip_geometry_sphere", invertNormals);
            ClipGeometryTypeTester::testType(sphere, reference, {}, invertNormals);
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
            auto transform = brayns::Transform{.translation = position, .scale = scale};

            auto reference = _filename("test_clip_geometry_mesh", invertNormals);
            ClipGeometryTypeTester::testType(mesh, reference, transform, invertNormals);
        }
    }

private:
    static std::string _filename(std::string_view baseName, bool invertNormals)
    {
        if (invertNormals)
        {
            return fmt::format("{}_inverted.png", baseName);
        }
        return fmt::format("{}.png", baseName);
    }
};
}

TEST_CASE("Clip geometry add/removal")
{
    auto utils = BraynsTestUtils();

    utils.addDefaultLights();
    utils.addGeometry(brayns::Sphere{brayns::Vector3f(0.f), 10.f}, {});
    utils.adjustPerspectiveView();

    ClippingBuilder::build(utils.getEngine(), brayns::Plane{{1.f, 0.f, 0.f, 0.f}});

    CHECK(ImageValidator::validate(utils.render(), "test_clip_geometry_add.png"));

    utils.removeClipping();

    CHECK(ImageValidator::validate(utils.render(), "test_clip_geometry_remove.png"));

    ClippingBuilder::build(utils.getEngine(), brayns::Plane{{-1.f, 0.f, 0.f, 0.f}});

    CHECK(ImageValidator::validate(utils.render(), "test_clip_geometry_re_add.png"));
}

TEST_CASE("Clip geometry types (outward normals)")
{
    ClipGeometryTester::test(false);
}

TEST_CASE("Clip geometry types (inward normals)")
{
    ClipGeometryTester::test(true);
}
