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
#include <brayns/engine/geometries/Plane.h>
#include <brayns/engine/lights/AmbientLight.h>
#include <brayns/engine/lights/DirectionalLight.h>

#include <tests/paths.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "helpers/ImageValidator.h"

struct BraynsInstance
{
    BraynsInstance(const bool orthographic)
        : braynsObject(0, nullptr)
    {
        auto &params = braynsObject.getParametersManager();
        auto &appParams = params.getApplicationParameters();
        appParams.setWindowSize(brayns::Vector2ui(300, 300));

        _setUpScene();
        _adjustCamera(orthographic);
    }

    brayns::Brayns braynsObject;

private:
    void _setUpScene()
    {
        const auto path = std::string(BRAYNS_TESTDATA_MODEL_PLY_PATH);
        const auto &loadRegistry = braynsObject.getLoaderRegistry();
        const auto &loader = loadRegistry.getSuitableLoader(path, "", "");
        auto models = loader.loadFromFile(path, {}, {});

        auto &engine = braynsObject.getEngine();

        auto &scene = engine.getScene();

        auto &modelManager = scene.getModelManager();
        modelManager.addModel({}, std::move(models.front()));

        auto &lightManager = scene.getLightManager();
        lightManager.addLight(std::make_unique<brayns::DirectionalLight>());

        scene.computeBounds();
    }

    void _adjustCamera(const bool orthographic)
    {
        auto &engine = braynsObject.getEngine();
        auto &scene = engine.getScene();
        const auto &bounds = scene.getBounds();
        auto center = bounds.center();
        auto dimensions = bounds.dimensions();

        if (orthographic)
        {
            auto orthographicCamera = std::make_unique<brayns::OrthographicCamera>();
            orthographicCamera->setHeight(dimensions.y);
            engine.setCamera(std::move(orthographicCamera));
        }
        else
        {
            engine.setCamera(std::make_unique<brayns::PerspectiveCamera>());
        }

        auto &camera = engine.getCamera();
        const auto position = center + brayns::Vector3f(0.f, 0.f, glm::compMax(dimensions));
        brayns::LookAt view{position, center, brayns::Vector3f(0.f, 1.f, 0.f)};
        camera.setLookAt(view);
    }
};

struct ClipPlaneGenerator
{
    static void add(const brayns::Vector4f &plane, brayns::SceneClipManager &manager)
    {
        auto model = std::make_unique<brayns::Model>();

        const auto planeGeometry = brayns::Plane{plane};
        model->addComponent<brayns::ClippingComponent<brayns::Plane>>(planeGeometry);

        manager.addClippingModel(std::move(model));
    }
};

void testClipping(bool orthographic)
{
    const std::string original = orthographic ? "clipPlaneRendering_ortho.png" : "clipPlaneRendering_perspective.png";

    const std::string clipped =
        orthographic ? "clipPlaneRendering_clipped_ortho.png" : "clipPlaneRendering_clipped_perspective.png";

    BraynsInstance instance(orthographic);
    auto &brayns = instance.braynsObject;
    auto &engine = brayns.getEngine();
    auto &scene = engine.getScene();
    const auto &bounds = scene.getBounds();
    const auto center = bounds.center();

    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, original));

    auto &clipManager = scene.getClipManager();
    ClipPlaneGenerator::add({0.f, 0.f, 1.f, center.z}, clipManager);
    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, clipped));

    clipManager.removeAllClippingModels();
    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, original));

    ClipPlaneGenerator::add({0.f, 0.f, 1.f, center.z}, clipManager);
    brayns.commitAndRender();
    CHECK(ImageValidator::validate(engine, clipped));

    clipManager.removeAllClippingModels();
}

TEST_CASE("perspective_clipping")
{
    testClipping(false);
}

TEST_CASE("orthographic_clipping")
{
    testClipping(true);
}
