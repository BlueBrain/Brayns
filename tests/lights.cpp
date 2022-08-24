/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/Brayns.h>

#include <brayns/engine/light/types/AmbientLight.h>
#include <brayns/engine/light/types/DirectionalLight.h>
#include <brayns/engine/light/types/QuadLight.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "helpers/BraynsTestUtils.h"
#include "helpers/ImageValidator.h"

TEST_CASE("render_ambient_light")
{
    brayns::Brayns brayns;

    BraynsTestUtils::addModel(brayns, BRAYNS_TESTDATA_MODEL_PLY_PATH);
    BraynsTestUtils::adjustPerspectiveView(brayns);

    auto intensity = 0.5f;
    auto color = brayns::Vector3f(1.f);
    BraynsTestUtils::addLight(brayns, brayns::Light(brayns::AmbientLight{intensity, color}));

    brayns.commitAndRender();

    CHECK(ImageValidator::validate(brayns.getEngine(), "testLightAmbient.png"));
}

TEST_CASE("render_directional_light")
{
    brayns::Brayns brayns;

    BraynsTestUtils::addModel(brayns, BRAYNS_TESTDATA_MODEL_PLY_PATH);
    BraynsTestUtils::adjustPerspectiveView(brayns);

    auto intensity = 5.5f;
    auto color = brayns::Vector3f(1.f);
    auto direction = glm::normalize(brayns::Vector3f(1.f, 1.f, 0.f));
    BraynsTestUtils::addLight(brayns, brayns::Light(brayns::DirectionalLight{intensity, color, direction}));

    brayns.commitAndRender();

    CHECK(ImageValidator::validate(brayns.getEngine(), "testLightDirectional.png"));
}

TEST_CASE("render_quad_light")
{
    brayns::Brayns brayns;

    BraynsTestUtils::addModel(brayns, BRAYNS_TESTDATA_MODEL_PLY_PATH);
    BraynsTestUtils::adjustPerspectiveView(brayns);

    auto &engine = brayns.getEngine();
    auto &scene = engine.getScene();
    const auto &bounds = scene.getBounds();

    auto dimensions = bounds.dimensions();

    auto intensity = 5.5f;
    auto color = brayns::Vector3f(1.f);
    auto position = bounds.getMax() + brayns::Vector3f(0.f, dimensions.y * 0.25f, 0.f);
    auto edge1 = brayns::Vector3f(-dimensions.x, 0.f, 0.f);
    auto edge2 = brayns::Vector3f(0.f, 0.f, -dimensions.z);
    auto quad = brayns::QuadLight{intensity, color, position, edge1, edge2};
    BraynsTestUtils::addLight(brayns, brayns::Light(quad));

    brayns.commitAndRender();

    CHECK(ImageValidator::validate(engine, "testLightQuad.png"));
}
