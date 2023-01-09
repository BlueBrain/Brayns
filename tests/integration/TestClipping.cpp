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
#include <brayns/engine/camera/projections/Orthographic.h>
#include <brayns/engine/camera/projections/Perspective.h>
#include <brayns/engine/components/ClipperViews.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/core/Engine.h>
#include <brayns/engine/geometry/types/Plane.h>
#include <brayns/engine/light/types/AmbientLight.h>
#include <brayns/engine/light/types/DirectionalLight.h>
#include <brayns/engine/scene/ModelsOperations.h>
#include <brayns/engine/systems/ClipperInitSystem.h>

#include <tests/paths.h>

#include "helpers/BraynsTestUtils.h"
#include "helpers/ImageValidator.h"

#include <doctest/doctest.h>

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

        auto model = std::make_shared<brayns::Model>("");

        auto planes = std::vector<brayns::Plane>{
            brayns::Plane{{planeA, planeADistance}},
            brayns::Plane{{planeB, planeBDistance}}};
        auto &components = model->getComponents();
        auto &clippers = components.add<brayns::Geometries>();
        clippers.elements.emplace_back(std::move(planes));

        auto &systems = model->getSystems();
        systems.setInitSystem<brayns::ClipperInitSystem>();

        auto &models = scene.getModels();
        models.add(std::move(model));
    }

    static void clear(brayns::Brayns &brayns)
    {
        auto &engine = brayns.getEngine();
        auto &scene = engine.getScene();
        auto &models = scene.getModels();
        brayns::ModelsOperations::removeClippers(models);
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

    BraynsTestUtils::addLight(brayns, brayns::Light(brayns::DirectionalLight()));
    BraynsTestUtils::addLight(brayns, brayns::Light(brayns::AmbientLight{0.05f}));

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

TEST_CASE("Perspective clipping")
{
    testClipping(false);
}

TEST_CASE("Orthographic clipping")
{
    testClipping(true);
}