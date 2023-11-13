/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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
#include <brayns/engine/colormethods/SolidColorMethod.h>
#include <brayns/engine/components/ColorRamp.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/Lights.h>
#include <brayns/engine/components/Volumes.h>
#include <brayns/engine/geometry/types/Box.h>
#include <brayns/engine/light/types/AmbientLight.h>
#include <brayns/engine/light/types/DirectionalLight.h>
#include <brayns/engine/scene/ModelsOperations.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryDataSystem.h>
#include <brayns/engine/systems/VolumeDataSystem.h>

/**
 * @brief Encapsulates some utilities used across all the tests
 */
class BraynsTestUtils
{
private:
    static inline auto args = "brayns";

public:
    explicit BraynsTestUtils():
        _brayns(1, &args)
    {
    }

    brayns::Brayns &getBrayns()
    {
        return _brayns;
    }

    brayns::Engine &getEngine()
    {
        return _brayns.getEngine();
    }

    template<typename T>
    brayns::ModelInstance *addGeometry(
        T geometry,
        const brayns::Transform &transform = {},
        const brayns::Vector4f &color = brayns::Vector4f(1.f))
    {
        auto model = std::make_shared<brayns::Model>("geometry");

        auto &components = model->getComponents();
        components.add<brayns::Geometries>(std::move(geometry));

        auto &systems = model->getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        systems.setDataSystem<brayns::GeometryDataSystem>();

        auto instance = addModel(std::move(model), transform);

        brayns::SolidColorMethod().apply(components, {{"color", color}});

        return instance;
    }

    template<typename T>
    brayns::ModelInstance *addVolume(
        T volume,
        const brayns::Transform &transform = {},
        const brayns::ColorRamp &colorRamp = {})
    {
        auto model = std::make_shared<brayns::Model>("volume");

        auto &components = model->getComponents();
        components.add<brayns::Volumes>(std::move(volume));
        components.add<brayns::ColorRamp>(std::move(colorRamp));

        auto &systems = model->getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Volumes>>();
        systems.setDataSystem<brayns::VolumeDataSystem>();

        return addModel(std::move(model), transform);
    }

    brayns::ModelInstance *addLight(brayns::Light light)
    {
        auto model = std::make_shared<brayns::Model>("light");

        auto &components = model->getComponents();
        components.add<brayns::Lights>(std::move(light));

        auto &systems = model->getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Lights>>();

        return addModel(std::move(model), {});
    }

    brayns::ModelInstance *addModel(std::shared_ptr<brayns::Model> model, const brayns::Transform &transform)
    {
        auto &engine = _brayns.getEngine();
        auto &scene = engine.getScene();
        auto &models = scene.getModels();
        auto instance = models.add(std::move(model));
        instance->setTransform(transform);
        return instance;
    }

    void loadModels(const std::string &path)
    {
        auto &loadRegistry = _brayns.getLoaderRegistry();
        auto format = brayns::LoaderFormat::from(path);
        auto *loader = loadRegistry.findByFormat(format);
        if (!loader)
        {
            throw std::runtime_error("Unsupported file");
        }
        auto params = brayns::RawFileLoaderRequest();
        params.path = path;
        auto loadedModels = loader->loadFile(params);

        auto &engine = _brayns.getEngine();
        auto &scene = engine.getScene();
        auto &models = scene.getModels();
        models.add(std::move(loadedModels));
    }

    void addDefaultGeometry()
    {
        addGeometry(brayns::Box{brayns::Vector3f(-5.f), brayns::Vector3f(-2.f)});
        addGeometry(brayns::Box{brayns::Vector3f(2.f), brayns::Vector3f(5.f)});
    }

    void addDefaultLights()
    {
        auto directional = brayns::DirectionalLight{
            10.f,
            brayns::Vector3f(1.f),
            brayns::math::normalize(brayns::Vector3f(1.f, -1.f, -1.f))};
        addLight(brayns::Light(directional));
        auto ambient = brayns::AmbientLight{0.2f};
        addLight(brayns::Light(ambient));
    }

    void createDefaultScene()
    {
        addDefaultGeometry();
        addDefaultLights();
    }

    void removeClipping()
    {
        auto &engine = _brayns.getEngine();
        auto &scene = engine.getScene();
        auto &models = scene.getModels();
        brayns::ModelsOperations::removeClippers(models);
    }

    void removeLights()
    {
        auto &engine = _brayns.getEngine();
        auto &scene = engine.getScene();
        auto &models = scene.getModels();
        brayns::ModelsOperations::removeLights(models);
    }

    brayns::Bounds getSceneBounds()
    {
        auto &engine = _brayns.getEngine();
        auto &scene = engine.getScene();
        return scene.getBounds();
    }

    void adjustPerspectiveView()
    {
        auto bounds = getSceneBounds();
        adjustPerspectiveView(bounds);
    }

    void adjustPerspectiveView(const brayns::Bounds &bounds)
    {
        auto projection = brayns::Perspective();

        auto center = bounds.center();
        auto size = bounds.dimensions();
        auto distance = size.y * 0.5f / brayns::math::tan(brayns::math::deg2rad(projection.fovy * 0.5f));
        auto position = center + brayns::Vector3f(0.f, 0.f, distance + size.z * 0.5f);
        auto view = brayns::View{position, center, brayns::Vector3f(0.f, 1.f, 0.f)};

        adjustCamera(projection, view);
    }

    void adjustOrthographicView()
    {
        auto bounds = getSceneBounds();

        auto center = bounds.center();
        auto dimensions = bounds.dimensions();
        auto distance = dimensions.z * 0.6f; // A bit more than just half to avoid any artifact
        auto position = center + brayns::Vector3f(0.f, 0.f, distance);
        auto view = brayns::View{position, center, brayns::Vector3f(0.f, 1.f, 0.f)};

        auto projection = brayns::Orthographic{dimensions.y};

        adjustCamera(projection, view);
    }

    template<typename T>
    void adjustCamera(T projection, const brayns::View &view)
    {
        auto &engine = _brayns.getEngine();
        auto &camera = engine.getCamera();
        camera.set(projection);
        camera.setView(view);
    }

    void setRenderResolution(uint32_t width, uint32_t height)
    {
        auto &params = _brayns.getParametersManager();
        auto &appParams = params.getApplicationParameters();
        appParams.setWindowSize(brayns::Vector2ui(width, height));
    }

    template<typename T>
    void setRenderer(T rendererType)
    {
        auto &engine = _brayns.getEngine();
        auto &renderer = engine.getRenderer();
        renderer.set(std::move(rendererType));
    }

    brayns::Image render()
    {
        auto &engine = _brayns.getEngine();
        engine.commitAndRender();

        auto &frameBuffer = engine.getFramebuffer();
        return frameBuffer.getImage();
    }

private:
    brayns::Brayns _brayns;
};
