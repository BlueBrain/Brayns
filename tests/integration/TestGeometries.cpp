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
#include <brayns/io/loaders/mesh/parsers/PlyMeshParser.h>
#include <brayns/utils/FileReader.h>

#include <tests/helpers/BraynsTestUtils.h>
#include <tests/helpers/ImageValidator.h>
#include <tests/paths.h>

#include <doctest/doctest.h>

namespace
{
class GeometryTypeTester
{
public:
    template<typename T>
    static void testType(T geometry, const std::string &filename, const brayns::Transform &transform = {})
    {
        auto utils = BraynsTestUtils();
        utils.addDefaultLights();
        utils.addGeometry(geometry, transform);
        utils.adjustPerspectiveView();

        CHECK(ImageValidator::validate(utils.render(), filename));
    }
};
}

TEST_CASE("Geometry types")
{
    SUBCASE("Bounded plane")
    {
        auto min = brayns::Vector3f(0.f);
        auto max = brayns::Vector3f(11.f);
        auto equation = brayns::Vector4f{0.f, 0.f, 1.f, 0.f};
        GeometryTypeTester::testType(brayns::BoundedPlane{equation, {min, max}}, "test_geometry_bounded_plane.png");
    }
    SUBCASE("Box")
    {
        auto min = brayns::Vector3f(-1.f);
        auto max = brayns::Vector3f(1.f);
        auto transform = brayns::Transform();
        transform.rotation = brayns::Quaternion(brayns::math::deg2rad(20.f), 0.f, 0.f);
        GeometryTypeTester::testType(brayns::Box{min, max}, "test_geometry_box.png", transform);
    }
    SUBCASE("Capsule")
    {
        auto p0 = brayns::Vector3f(0.f, -10.f, 7.5f);
        auto r0 = 6.f;
        auto p1 = brayns::Vector3f(0.f, 10.f, 7.5f);
        auto r1 = 3.f;
        GeometryTypeTester::testType(brayns::Capsule{p0, r0, p1, r1}, "test_geometry_capsule.png");
    }
    SUBCASE("Plane")
    {
        auto equation = brayns::Vector4f(0.f, 0.f, -1.f, 0.f);

        auto transform1 = brayns::Transform();
        transform1.rotation = brayns::Quaternion(brayns::math::deg2rad(-40.f), 0.f, 0.f);

        auto transform2 = brayns::Transform();
        transform2.rotation = brayns::Quaternion(brayns::math::deg2rad(40.f), 0.f, 0.f);

        auto utils = BraynsTestUtils();
        utils.addDefaultLights();
        utils.addGeometry(brayns::Plane{equation}, transform1);
        utils.addGeometry(brayns::Plane{equation}, transform2, brayns::Vector4f(1.f, 0.f, 0.f, 1.f));

        // Planes do not have bounds
        auto bounds = brayns::Bounds(brayns::Vector3f(-5.f), brayns::Vector3f(5.f));
        utils.adjustPerspectiveView(bounds);

        CHECK(ImageValidator::validate(utils.render(), "test_geometry_plane.png"));
    }
    SUBCASE("Sphere")
    {
        auto center = brayns::Vector3f(0.f, 0.f, 6.f);
        auto radius = 7.f;
        GeometryTypeTester::testType(brayns::Sphere{center, radius}, "test_geometry_sphere.png");
    }
    SUBCASE("Triangle mesh")
    {
        auto content = brayns::FileReader::read(TestPaths::Meshes::lucy);
        auto mesh = brayns::PlyMeshParser().parse(content);
        GeometryTypeTester::testType(mesh, "test_geometry_mesh.png");
    }
}
