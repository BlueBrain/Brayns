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

#include <doctest/doctest.h>

#include <brayns/engine/geometry/types/Sphere.h>
#include <brayns/engine/material/types/CarPaint.h>
#include <brayns/engine/material/types/Emissive.h>
#include <brayns/engine/material/types/Ghost.h>
#include <brayns/engine/material/types/Glass.h>
#include <brayns/engine/material/types/Matte.h>
#include <brayns/engine/material/types/Metal.h>
#include <brayns/engine/material/types/Phong.h>
#include <brayns/engine/material/types/Plastic.h>
#include <brayns/engine/material/types/Principled.h>
#include <brayns/engine/renderer/types/Production.h>

#include <tests/helpers/BraynsTestUtils.h>
#include <tests/helpers/ImageValidator.h>

namespace
{
class MaterialPainter
{
public:
    template<typename T>
    static void apply(brayns::ModelInstance *instance, T material)
    {
        auto &model = instance->getModel();
        auto &components = model.getComponents();
        components.add<brayns::Material>(material);
    }
};

class MaterialTypeTester
{
public:
    template<typename T>
    static void test(T materialType, const std::string &filename)
    {
        auto utils = BraynsTestUtils();

        utils.addDefaultLights();
        auto geometry = brayns::Sphere{brayns::Vector3f(0.f), 1.f};
        auto color = brayns::Vector4f(1.f, 0.f, 0.f, 1.f);
        auto instance = utils.addGeometry(geometry, {}, color);
        MaterialPainter::apply(instance, materialType);

        utils.adjustPerspectiveView();
        utils.setRenderer(brayns::Production{16, 3});

        CHECK(ImageValidator::validate(utils.render(), filename));
    }
};
}

TEST_CASE("Material change")
{
    auto utils = BraynsTestUtils();

    utils.addDefaultLights();
    auto geometry = brayns::Sphere{brayns::Vector3f(0.f), 1.f};
    auto color = brayns::Vector4f(1.f, 0.f, 0.f, 1.f);
    auto instance = utils.addGeometry(geometry, {}, color);
    utils.adjustPerspectiveView();
    utils.setRenderer(brayns::Production{16, 3});

    CHECK(ImageValidator::validate(utils.render(), "test_material_default.png"));

    MaterialPainter::apply(instance, brayns::Metal());
    CHECK(ImageValidator::validate(utils.render(), "test_material_add_first.png"));

    MaterialPainter::apply(instance, brayns::Matte());
    CHECK(ImageValidator::validate(utils.render(), "test_material_replace.png"));
}

TEST_CASE("Material types")
{
    SUBCASE("CarPaint")
    {
        MaterialTypeTester::test(brayns::CarPaint{0.5f}, "test_material_carpaint.png");
    }
    SUBCASE("Emissive")
    {
        MaterialTypeTester::test(brayns::Emissive{brayns::Vector3f(0.f, 1.f, 0.f)}, "test_material_emissive.png");
    }
    SUBCASE("Ghost")
    {
        MaterialTypeTester::test(brayns::Ghost(), "test_material_ghost.png");
    }
    SUBCASE("Glass")
    {
        MaterialTypeTester::test(brayns::Glass(), "test_material_glass.png");
    }
    SUBCASE("Matte")
    {
        MaterialTypeTester::test(brayns::Matte(), "test_material_matte.png");
    }
    SUBCASE("Metal")
    {
        MaterialTypeTester::test(brayns::Metal{0.5f}, "test_material_metal.png");
    }
    SUBCASE("Phong")
    {
        MaterialTypeTester::test(brayns::Phong(), "test_material_phong.png");
    }
    SUBCASE("Plastic")
    {
        MaterialTypeTester::test(brayns::Plastic(), "test_material_plastic.png");
    }
    SUBCASE("Principled")
    {
        auto polishedBallMaterial = brayns::Principled{
            .edgeColor = brayns::Vector3f(.8f),
            .metallic = .9f,
            .roughness = .6f,
            .anisotropy = 1.f,
            .anisotropyRotation = .9f};
        MaterialTypeTester::test(polishedBallMaterial, "test_material_principled.png");
    }
}
