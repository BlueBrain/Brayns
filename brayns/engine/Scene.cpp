/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "Scene.h"

#include <ospray/ospray_cpp/Data.h>

namespace
{
class WorldParameterUpdater
{
public:
    static void updateInstances(const ospray::cpp::World &world, const std::vector<ospray::cpp::Instance> &instances)
    {
        static const std::string instanceParameter = "instance";

        if (instances.empty())
        {
            world.removeParam(instanceParameter);
            return;
        }
        world.setParam(instanceParameter, ospray::cpp::CopiedData(instances));
    }

    static void updateLights(const ospray::cpp::World &world, const std::vector<ospray::cpp::Light> &lights)
    {
        static const std::string lightParameter = "light";

        if (lights.empty())
        {
            world.removeParam(lightParameter);
            return;
        }
        world.setParam(lightParameter, ospray::cpp::CopiedData(lights));
    }
};
}

namespace brayns
{
Scene::Scene()
{
    // Need an initial commit until there is any content that can trigger the commit function
    // ospCommit(_handle);
    Log::critical("TESTING SCENE COMMIT ON CONSTRUCTOR");
}

const Bounds &Scene::getBounds() const noexcept
{
    return _bounds;
}

void Scene::computeBounds() noexcept
{
    _bounds = Bounds();
    _bounds.expand(_modelManager.getBounds());
    _bounds.expand(_lightManager.getBounds());
}

void Scene::preRender(const ParametersManager &params)
{
    _modelManager.preRender(params);
}

void Scene::postRender(const ParametersManager &params)
{
    _modelManager.postRender(params);
}

bool Scene::commit()
{
    bool needsCommit = false;

    // Commit models
    if (_modelManager.commit() || _clippingManager.commit())
    {
        std::vector<ospray::cpp::Instance> instances;
        auto modelInstances = _modelManager.getOsprayInstances();
        instances.insert(instances.end(), modelInstances.begin(), modelInstances.end());
        auto clipInstances = _clippingManager.getOsprayInstances();
        instances.insert(instances.end(), clipInstances.begin(), clipInstances.end());
        WorldParameterUpdater::updateInstances(_osprayWorld, instances);
        needsCommit = true;
    }

    if (_lightManager.commit())
    {
        auto lights = _lightManager.getOsprayLights();
        WorldParameterUpdater::updateLights(_osprayWorld, lights);
        needsCommit = true;
    }

    // Commit handle
    if (needsCommit)
    {
        _osprayWorld.commit();
    }

    return needsCommit;
}

ModelInstance &Scene::addModel(ModelLoadParameters params, std::unique_ptr<Model> model)
{
    return _modelManager.addModel(std::move(params), std::move(model));
}

std::vector<ModelInstance *> Scene::addModels(ModelLoadParameters params, std::vector<std::unique_ptr<Model>> models)
{
    auto instances = _modelManager.addModels(std::move(params), std::move(models));
    computeBounds();
    return instances;
}

void Scene::removeModelInstances(const std::vector<uint32_t> &instanceIds)
{
    _modelManager.removeModels(instanceIds);
    computeBounds();
}

void Scene::removeAllModelInstances() noexcept
{
    _modelManager.removeAllModelInstances();
    computeBounds();
}

ModelInstance &Scene::getModelInstance(uint32_t instanceId)
{
    return _modelManager.getModelInstance(instanceId);
}

const std::vector<ModelInstance *> &Scene::getAllModelInstances() const noexcept
{
    return _modelManager.getAllModelInstances();
}

const ModelLoadParameters &Scene::getModelLoadParameters(uint32_t instanceId) const
{
    return _modelManager.getModelLoadParameters(instanceId);
}

uint32_t Scene::addLight(std::unique_ptr<Light> light)
{
    auto lightId = _lightManager.addLight(std::move(light));
    computeBounds();
    return lightId;
}

void Scene::removeLights(const std::vector<uint32_t> &lightIds)
{
    _lightManager.removeLights(lightIds);
    computeBounds();
}

void Scene::removeAllLights() noexcept
{
    _lightManager.removeAllLights();
    computeBounds();
}

uint32_t Scene::addClippingModel(std::unique_ptr<Model> model)
{
    return _clippingManager.addClippingModel(std::move(model));
}

void Scene::removeClippingModels(const std::vector<uint32_t> &modelIds)
{
    _clippingManager.removeClippingModels(modelIds);
}

void Scene::removeAllClippingModels() noexcept
{
    _clippingManager.removeAllClippingModels();
}

const ospray::cpp::World &Scene::getOsprayScene() const noexcept
{
    return _osprayWorld;
}
} // namespace brayns
