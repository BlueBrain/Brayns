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

#include <brayns/engine/lights/AmbientLight.h>
#include <brayns/engine/lights/DirectionalLight.h>
#include <brayns/engine/lights/QuadLight.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "helpers/BraynsTestUtils.h"
#include "helpers/ImageValidator.h"

TEST_CASE("render_ambient_light")
{
    brayns::Brayns brayns;

    BraynsTestUtils::addModel(brayns, BRAYNS_TESTDATA_MODEL_PLY_PATH);
    BraynsTestUtils::adjustPerspectiveView(brayns);

    auto light = std::make_unique<brayns::AmbientLight>();

    const auto lightIntensity = 0.5f;
    light->setIntensity(lightIntensity);

    const auto lightColor = brayns::Vector3f(1.f);
    light->setColor(lightColor);

    BraynsTestUtils::addLight(brayns, std::move(light));

    brayns.commitAndRender();

    auto &engine = brayns.getEngine();
    CHECK(ImageValidator::validate(engine, "testLightAmbient.png"));
}

TEST_CASE("render_directional_light")
{
    brayns::Brayns brayns;

    BraynsTestUtils::addModel(brayns, BRAYNS_TESTDATA_MODEL_PLY_PATH);
    BraynsTestUtils::adjustPerspectiveView(brayns);

    auto &engine = brayns.getEngine();

    const auto lightDirection = glm::normalize(brayns::Vector3f(1.f, 1.f, 0.f));
    const auto lightIntensity = 5.5f;
    const auto lightColor = brayns::Vector3f(1.f);

    auto light = std::make_unique<brayns::DirectionalLight>();
    light->setDirection(lightDirection);
    light->setIntensity(lightIntensity);
    light->setColor(lightColor);
    BraynsTestUtils::addLight(brayns, std::move(light));

    brayns.commitAndRender();

    CHECK(ImageValidator::validate(engine, "testLightDirectional.png"));
}

TEST_CASE("render_quad_light")
{
    brayns::Brayns brayns;

    BraynsTestUtils::addModel(brayns, BRAYNS_TESTDATA_MODEL_PLY_PATH);
    BraynsTestUtils::adjustPerspectiveView(brayns);

    auto &engine = brayns.getEngine();
    auto &scene = engine.getScene();
    const auto &bounds = scene.getBounds();

    const auto dimensions = bounds.dimensions();
    const auto lightCorner = bounds.getMax() + brayns::Vector3f(0.f, dimensions.y * 0.25f, 0.f);
    const auto lightHorizontalVector = brayns::Vector3f(-dimensions.x, 0.f, 0.f);
    const auto lightVerticalVector = brayns::Vector3f(0.f, 0.f, -dimensions.z);
    const auto lightIntensity = 5.5f;
    const auto lightColor = brayns::Vector3f(1.f);

    auto light = std::make_unique<brayns::QuadLight>();
    light->setPosition(lightCorner);
    light->setEdge1(lightHorizontalVector);
    light->setEdge2(lightVerticalVector);
    light->setIntensity(lightIntensity);
    light->setColor(lightColor);
    BraynsTestUtils::addLight(brayns, std::move(light));

    brayns.commitAndRender();

    CHECK(ImageValidator::validate(engine, "testLightQuad.png"));
}
