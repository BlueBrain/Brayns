/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/engine/light/types/AmbientLight.h>
#include <brayns/engine/light/types/DirectionalLight.h>
#include <brayns/engine/light/types/QuadLight.h>
#include <brayns/io/loaders/mesh/parsers/PlyMeshParser.h>
#include <brayns/utils/FileReader.h>

#include <tests/helpers/BraynsTestUtils.h>
#include <tests/helpers/ImageValidator.h>
#include <tests/paths.h>

#include <doctest/doctest.h>

namespace
{
class LightTypeTester
{
public:
    template<typename T>
    static void test(T lightType, const std::string &filename)
    {
        auto utils = BraynsTestUtils();
        utils.loadModels(TestPaths::Meshes::lucy);
        utils.addLight(brayns::Light(lightType));
        utils.adjustPerspectiveView();

        CHECK(ImageValidator::validate(utils.render(), filename));
    }
};

class TestMeshBounds
{
public:
    static brayns::Bounds compute()
    {
        auto content = brayns::FileReader::read(TestPaths::Meshes::lucy);
        auto mesh = brayns::PlyMeshParser().parse(content);
        return brayns::GeometryTraits<brayns::TriangleMesh>::computeBounds({}, mesh);
    }
};
}

TEST_CASE("Light add/removal")
{
    auto utils = BraynsTestUtils();
    utils.addLight(brayns::Light(brayns::AmbientLight()));
    utils.loadModels(TestPaths::Meshes::lucy);
    utils.adjustPerspectiveView();

    CHECK(ImageValidator::validate(utils.render(), "test_lights_add.png"));

    utils.removeLights();

    CHECK(ImageValidator::validate(utils.render(), "test_lights_remove.png"));

    utils.addLight(brayns::Light(brayns::AmbientLight()));

    CHECK(ImageValidator::validate(utils.render(), "test_lights_readd.png"));
}

TEST_CASE("Light types")
{
    SUBCASE("Ambient light")
    {
        LightTypeTester::test(brayns::AmbientLight(), "test_light_ambient.png");
    }
    SUBCASE("Directional light")
    {
        auto direction = brayns::math::normalize(brayns::Vector3f(1.f, -1.f, -1.f));
        auto intensity = 10.f;
        auto color = brayns::Vector3f(0.5f, 0.5f, 1.f);
        LightTypeTester::test(brayns::DirectionalLight{intensity, color, direction}, "test_light_directional.png");
    }
    SUBCASE("Quad light")
    {
        auto bounds = TestMeshBounds::compute();
        auto &min = bounds.getMin();
        auto &max = bounds.getMax();
        auto size = bounds.dimensions();

        auto position = brayns::Vector3f(min.x, max.y + size.y * 0.1f, min.z);
        auto edge1 = brayns::Vector3f(size.x, 0.f, 0.f);
        auto edge2 = brayns::Vector3f(0.f, 0.f, size.z);
        auto intensity = 10.f;
        auto color = brayns::Vector3f(0.5f, 1.f, 0.5f);
        auto quad = brayns::QuadLight{intensity, color, position, edge1, edge2};
        LightTypeTester::test(quad, "test_light_quad.png");
    }
}
