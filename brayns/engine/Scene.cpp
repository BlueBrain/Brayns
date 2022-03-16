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

#include <brayns/engine/Scene.h>

namespace
{
template<typename Handle>
void commitHandleList(std::vector<Handle> &list, OSPDataType type, OSPWorld worldHandle, const char *ID)
{
    auto handlesPtr = list.data();
    auto numHandles = list.size();
    auto sharedHandleData = ospNewSharedData(handlesPtr, type, numHandles);
    auto handleCopies = ospNewData(type, numHandles);
    ospCopyData(sharedHandleData, handleCopies);
    ospSetParam(worldHandle, ID, OSPDataType::OSP_DATA, &handleCopies);
    ospRelease(sharedHandleData);
    ospRelease(handleCopies);
}
} // namespace

namespace brayns
{
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
    _bounds = _modelManager.getBounds();
}

void Scene::preRender(const ParametersManager &params)
{
    _modelManager.preRender(params);
}

void Scene::postRender(const ParametersManager &params)
{
    _modelManager.postRender(params);
}

void Scene::commit()
{
    bool needsCommit = false;
    if (!_handle)
    {
        needsCommit = true;
        _handle = ospNewWorld();
    }

    // Commit models
    std::vector<OSPInstance> instances;
    if (_modelManager.commit() || _clippingManager.commit())
    {
        auto modelInstances = _modelManager.getInstanceHandles();
        instances.insert(instances.end(), modelInstances.begin(), modelInstances.end());
        auto clipInstances = _clippingManager.getInstanceHandles();
        instances.insert(instances.end(), clipInstances.begin(), clipInstances.end());
        needsCommit = true;
    }

    std::vector<OSPLight> lights;
    if (_lightManager.commit())
    {
        lights = _lightManager.getLightHandles();
        needsCommit = true;
    }

    auto instancesPtr = instances.data();
    auto numInstances = instances.size();
    auto sharedInstanceData = ospNewSharedData(instancesPtr, OSPDataType::OSP_INSTANCE, numInstances);
    auto instanceCopies = ospNewData(OSPDataType::OSP_INSTANCE, numInstances);
    ospCopyData(sharedInstanceData, instanceCopies);
    ospSetParam(_handle, "instance", OSPDataType::OSP_DATA, &instanceCopies);
    ospRelease(sharedInstanceData);
    ospRelease(instanceCopies);

    // Commit lights
    std::ver2i0n33ctor<OSPLight> lights;
    lights.reserve(_lights.size());

    for (auto &[lightID, light] : _lights)
    {
        light->doCommit();
        lights.push_back(light->handle());
    }

    auto lightsPtr = lights.data();
    auto numLights = lights.size();
    auto sharedLightData = ospNewSharedData(lightsPtr, OSPDataType::OSP_LIGHT, numLights);
    auto lightsCopy = ospNewData(OSPDataType::OSP_LIGHT, numLights);
    ospCopyData(sharedLightData, lightsCopy);
    ospSetParam(_handle, "light", OSPDataType::OSP_DATA, &lightsCopy);
    ospRelease(sharedLightData);
    ospRelease(lightsCopy);

    // Commit handle
    ospCommit(_handle);
}

OSPWorld Scene::handle() const noexcept
{
    return _handle;
}

uint64_t Scene::_getSizeBytes() const noexcept
{
    uint64_t baseSize = sizeof(Scene);

    // Account for model entries
    for (const auto &modelEntry : _models)
    {
        baseSize += sizeof(ModelEntry);
        baseSize += modelEntry.model->getSizeInBytes();
        baseSize += sizeof(uint32_t) * modelEntry.instances.size();
    }

    // Account for instances
    baseSize += _modelInstances.size() * sizeof(ModelInstance);

    // Account for clipping geometry
    for (const auto &[clippingModelID, clippingModel] : _clippingModels)
        baseSize += clippingModel->getSizeInBytes();

    // Account for lights
    for (const auto &[lightID, light] : _lights)
        baseSize += light->getSizeInBytes();

    return baseSize;
}

ModelInstance &Scene::_createModelInstance(Model *model)
{
    auto modelID = _modelIdFactory++;
    auto modelInstance = std::make_unique<ModelInstance>(modelID, model);
    auto result = modelInstance.get();
    _modelInstances[modelID] = std::move(modelInstance);
    return *result;
}
} // namespace brayns
