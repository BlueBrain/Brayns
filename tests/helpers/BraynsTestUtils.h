/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#pragma once

#include <brayns/Brayns.h>

#include <brayns/engine/cameras/OrthographicCamera.h>
#include <brayns/engine/cameras/PerspectiveCamera.h>

/**
 * @brief Encapsulates some utilities used across all the tests
 */
class BraynsTestUtils
{
public:
    static void setRenderResolution(brayns::Brayns &brayns, uint32_t width, uint32_t height)
    {
        auto &params = brayns.getParametersManager();
        auto &appParams = params.getApplicationParameters();
        appParams.setWindowSize(brayns::Vector2ui(width, height));
    }

    static void addLight(brayns::Brayns &brayns, std::unique_ptr<brayns::Light> light)
    {
        auto &engine = brayns.getEngine();
        auto &scene = engine.getScene();
        auto &lightManager = scene.getLightManager();
        lightManager.addLight(std::move(light));
    }

    static void addModel(brayns::Brayns &brayns, const std::string &path)
    {
        const auto &loadRegistry = brayns.getLoaderRegistry();
        const auto &loader = loadRegistry.getSuitableLoader(path, "", "");
        auto models = loader.loadFromFile(path, {}, {});
        auto &engine = brayns.getEngine();
        auto &scene = engine.getScene();
        auto &modelManager = scene.getModelManager();
        modelManager.addModel({}, std::move(models.front()));
        scene.computeBounds();
    }

    static void adjustPerspectiveView(brayns::Brayns &brayns)
    {
        auto camera = std::make_unique<brayns::PerspectiveCamera>();
        auto &engine = brayns.getEngine();

        constexpr auto fovy = 45.f;
        camera->setFOVY(fovy);

        auto &scene = engine.getScene();
        const auto &bounds = scene.getBounds();
        auto center = bounds.center();
        auto dimensions = bounds.dimensions();
        auto distance = dimensions.y * 0.5 / glm::tan(glm::radians(fovy * 0.5));
        const auto position = center + brayns::Vector3f(0.f, 0.f, distance);
        brayns::LookAt view{position, center, brayns::Vector3f(0.f, 1.f, 0.f)};
        camera->setLookAt(view);

        engine.setCamera(std::move(camera));
    }

    static void adjustOrthographicView(brayns::Brayns &brayns)
    {
        auto camera = std::make_unique<brayns::OrthographicCamera>();

        auto &engine = brayns.getEngine();
        auto &scene = engine.getScene();
        const auto &bounds = scene.getBounds();
        auto center = bounds.center();
        auto dimensions = bounds.dimensions();

        const auto distance = dimensions.z * 0.6; // A bit more than just half to avoid any artifact
        const auto position = center + brayns::Vector3f(0.f, 0.f, distance);
        brayns::LookAt view{position, center, brayns::Vector3f(0.f, 1.f, 0.f)};
        camera->setLookAt(view);

        const auto height = dimensions.y;
        camera->setHeight(height);

        engine.setCamera(std::move(camera));
    }
};
