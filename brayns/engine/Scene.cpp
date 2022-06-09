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

#include <brayns/engine/common/DataHandler.h>

namespace
{
class InstanceCommitter
{
public:
    static void commit(OSPWorld world, const std::vector<OSPInstance> &instances)
    {
        if (instances.empty())
        {
            ospRemoveParam(world, "instance");
            return;
        }

        auto instanceBuffer = brayns::DataHandler::copyBuffer(instances, OSPDataType::OSP_INSTANCE);
        ospSetParam(world, "instance", OSPDataType::OSP_DATA, &instanceBuffer.handle);
    }
};

class LightComitter
{
public:
    static void commit(OSPWorld world, const std::vector<OSPLight> &lights)
    {
        if (lights.empty())
        {
            ospRemoveParam(world, "light");
            return;
        }

        auto lightBuffer = brayns::DataHandler::copyBuffer(lights, OSPDataType::OSP_LIGHT);
        ospSetParam(world, "light", OSPDataType::OSP_DATA, &lightBuffer.handle);
    }
};
}

namespace brayns
{
Scene::Scene()
    : _bounds(Vector3f(0.f), Vector3f(0.f))
    , _handle(ospNewWorld())
{
    // Need an initial commit until there is any content that can trigger the commit function
    ospCommit(_handle);
}

Scene::~Scene()
{
    ospRelease(_handle);
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
        std::vector<OSPInstance> instances;
        auto modelInstances = _modelManager.getInstanceHandles();
        instances.insert(instances.end(), modelInstances.begin(), modelInstances.end());
        auto clipInstances = _clippingManager.getInstanceHandles();
        instances.insert(instances.end(), clipInstances.begin(), clipInstances.end());
        InstanceCommitter::commit(_handle, instances);
        needsCommit = true;
    }

    if (_lightManager.commit())
    {
        auto lights = _lightManager.getLightHandles();
        LightComitter::commit(_handle, lights);
        needsCommit = true;
    }

    // Commit handle
    if (needsCommit)
    {
        ospCommit(_handle);
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

OSPWorld Scene::handle() const noexcept
{
    return _handle;
}
} // namespace brayns
