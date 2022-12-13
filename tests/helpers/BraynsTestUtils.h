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

#include <brayns/engine/camera/projections/Orthographic.h>
#include <brayns/engine/camera/projections/Perspective.h>

#include <brayns/engine/components/Lights.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/geometry/types/Box.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryCommitSystem.h>
#include <brayns/engine/systems/GeometryInitSystem.h>

/**
 * @brief Encapsulates some utilities used across all the tests
 */
class BraynsTestUtils
{
public:
    static brayns::ModelInstance *addCube(
        brayns::Brayns &brayns,
        const brayns::Vector3f &position = brayns::Vector3f(0.f),
        const brayns::Vector3f &size = brayns::Vector3f(1.f),
        const brayns::Vector4f &color = brayns::Vector4f(1.f, 0.f, 0.f, 1.f))
    {
        auto cube = brayns::Box{position, position + size};

        auto model = std::make_shared<brayns::Model>("");

        auto &components = model->getComponents();
        auto &geometries = components.add<brayns::Geometries>(cube);
        auto &views = components.add<brayns::GeometryViews>(geometries.elements);
        auto &view = views.elements.front();
        view.setColor(color);

        auto &systems = model->getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        systems.setCommitSystem<brayns::GeometryCommitSystem>();
        systems.setInitSystem<brayns::GeometryInitSystem>();

        auto &engine = brayns.getEngine();
        auto &scene = engine.getScene();
        auto &models = scene.getModels();
        return models.add(std::move(model));
    }

    static void adjustPerspective(brayns::Camera &camera, const brayns::Bounds &bounds)
    {
        auto &projection = *camera.as<brayns::Perspective>();
        auto center = bounds.center();
        auto size = bounds.dimensions();
        auto distance = size.y * 0.5 / glm::tan(glm::radians(projection.fovy * 0.5));
        auto position = center + brayns::Vector3f(0.f, 0.f, distance + size.z * 0.5f);
        auto view = brayns::View{position, center, brayns::Vector3f(0.f, 1.f, 0.f)};
        camera.setView(view);
        camera.commit();
    }

    static void setRenderResolution(brayns::Brayns &brayns, uint32_t width, uint32_t height)
    {
        auto &params = brayns.getParametersManager();
        auto &appParams = params.getApplicationParameters();
        appParams.setWindowSize(brayns::Vector2ui(width, height));
    }

    static void addLight(brayns::Brayns &brayns, brayns::Light light)
    {
        auto &engine = brayns.getEngine();
        auto &scene = engine.getScene();
        auto &models = scene.getModels();

        auto model = std::make_shared<brayns::Model>("");

        auto &components = model->getComponents();
        auto &lights = components.add<brayns::Lights>();
        lights.elements.push_back(std::move(light));

        auto &systems = model->getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Lights>>();

        models.add(std::move(model));
    }

    static void addModel(brayns::Brayns &brayns, const std::string &path)
    {
        const auto &loadRegistry = brayns.getLoaderRegistry();
        const auto &loader = loadRegistry.getSuitableLoader(path, "", "");
        auto loadedModels = loader.loadFromFile(path, {}, {});
        auto &engine = brayns.getEngine();
        auto &scene = engine.getScene();
        auto &models = scene.getModels();
        models.add(std::move(loadedModels));
    }

    static void adjustPerspectiveView(brayns::Brayns &brayns)
    {
        brayns::Perspective projection{45.f};

        auto &engine = brayns.getEngine();
        auto &scene = engine.getScene();
        const auto &bounds = scene.getBounds();
        auto center = bounds.center();
        auto dimensions = bounds.dimensions();
        auto distance = dimensions.y * 0.5 / glm::tan(glm::radians(projection.fovy * 0.5));
        const auto position = center + brayns::Vector3f(0.f, 0.f, distance + dimensions.z * 0.5f);
        auto view = brayns::View{position, center, brayns::Vector3f(0.f, 1.f, 0.f)};

        auto &camera = engine.getCamera();
        camera.set(projection);
        camera.setView(view);
    }

    static void adjustOrthographicView(brayns::Brayns &brayns)
    {
        auto &engine = brayns.getEngine();
        auto &scene = engine.getScene();
        const auto &bounds = scene.getBounds();
        auto center = bounds.center();
        auto dimensions = bounds.dimensions();

        const auto distance = dimensions.z * 0.6; // A bit more than just half to avoid any artifact
        const auto position = center + brayns::Vector3f(0.f, 0.f, distance);
        auto view = brayns::View{position, center, brayns::Vector3f(0.f, 1.f, 0.f)};
        auto projection = brayns::Orthographic{dimensions.y};

        auto &camera = engine.getCamera();
        camera.set(projection);
        camera.setView(view);
    }
};
