/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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
#include <brayns/engine/Engine.h>
#include <brayns/engine/cameras/OrthographicCamera.h>
#include <brayns/engine/cameras/PerspectiveCamera.h>
#include <brayns/engine/components/ClippingComponent.h>
#include <brayns/engine/geometry/types/Plane.h>
#include <brayns/engine/lights/AmbientLight.h>
#include <brayns/engine/lights/DirectionalLight.h>

#include <tests/paths.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "helpers/BraynsTestUtils.h"
#include "helpers/ImageValidator.h"

namespace
{
struct ZParallelSliceManager
{
    static void generate(brayns::Brayns &brayns)
    {
        auto &engine = brayns.getEngine();
        auto &scene = engine.getScene();
        const auto &bounds = scene.getBounds();
        const auto center = bounds.center();
        const auto dimensions = bounds.dimensions();
        const auto sliceThickness = dimensions.z * 0.1f;

        // Clip every point that dot(planeA, point) + planeADistance < 0.0
        const auto planeA = brayns::Vector3f(0.f, 0.f, 1.f);
        const auto planeADistance = center.z - sliceThickness * 0.5f;

        // Clip every point that dot(planeB, point) + planeBDistance < 0.0
        const auto planeB = brayns::Vector3f(0.f, 0.f, -1.f);
        const auto planeBDistance = -center.z - sliceThickness * 0.5f;

        auto model = std::make_unique<brayns::Model>();
        const auto planes = std::vector<brayns::Vector4f>{{planeA, planeADistance}, {planeB, planeBDistance}};
        for (const auto &plane : planes)
        {
            const auto planeGeometry = brayns::Plane{plane};
            model->addComponent<brayns::ClippingComponent<brayns::Plane>>(planeGeometry);
        }

        scene.addClippingModel(std::move(model));
    }

    static void clear(brayns::Brayns &brayns)
    {
        auto &engine = brayns.getEngine();
        auto &scene = engine.getScene();
        scene.removeAllClippingModels();
    }
};
} // namespace

void testClipping(bool orthographic)
{
    const auto original = orthographic ? "testClipBaseOrtho.png" : "testClipBasePerspective.png";
    const auto clipped = orthographic ? "testClipClippedOrtho.png" : "testClipClippedPerspective.png";

    brayns::Brayns brayns;

    BraynsTestUtils::setRenderResolution(brayns, 300, 300);
    BraynsTestUtils::addModel(brayns, BRAYNS_TESTDATA_MODEL_PLY_PATH);

    if (orthographic)
    {
        BraynsTestUtils::adjustOrthographicView(brayns);
    }
    else
    {
        BraynsTestUtils::adjustPerspectiveView(brayns);
    }

    BraynsTestUtils::addLight(brayns, std::make_unique<brayns::DirectionalLight>());
    auto ambientLight = std::make_unique<brayns::AmbientLight>();
    ambientLight->setIntensity(0.05f);
    BraynsTestUtils::addLight(brayns, std::move(ambientLight));

    auto &engine = brayns.getEngine();
    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, original));

    ZParallelSliceManager::generate(brayns);
    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, clipped));
    ZParallelSliceManager::clear(brayns);

    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, original));

    ZParallelSliceManager::generate(brayns);
    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, clipped));
    ZParallelSliceManager::clear(brayns);
}

TEST_CASE("perspective_clipping")
{
    testClipping(false);
}

TEST_CASE("orthographic_clipping")
{
    testClipping(true);
}
