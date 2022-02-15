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
template<typename ModelMap>
auto& retrieveModel(ModelMap& models, const uint32_t modelID)
{
    auto it = models.find(modelID);
    if(it == models.end())
        throw std::invalid_argument("Model with ID " + std::to_string(modelID) + " does not exists");

    return *(it->second);
}
}

namespace brayns
{
Scene::Scene()
{
    _handle = ospNewWorld();
}

Scene::~Scene()
{
    ospRelease(_handle);
}

const Bounds &Scene::getBounds() const noexcept
{
    return _bounds;
}

std::vector<ModelInstance*> Scene::addModels(ModelsLoadParameters params, std::vector<Model::Ptr>&& models)
{
    if(models.empty())
        throw std::invalid_argument("Attempted to add an empty of list models to the scene");

    _loadEntries.emplace_back();
    auto& loadEntry = _loadEntries.back();

    loadEntry.params = std::move(params);

    auto& modelIndices = loadEntry.modelIndices;
    modelIndices.reserve(models.size());

    std::vector<ModelInstance*> result;
    result.reserve(models.size());

    std::lock_guard lock(_loadMutex);

    for(size_t i = 0; i < models.size(); ++i)
    {
        auto& model = models[i];

        auto modelID = _modelIdFactory++;
        auto modelInstance = std::make_unique<ModelInstance>(modelID, std::move(model));
        result.push_back(modelInstance.get());
        _models[modelID] = std::move(modelInstance);

        auto modelIndex = static_cast<uint32_t>(i);
        modelIndices.push_back({modelID, modelIndex});
    }

    markModified(false);

    return result;
}

ModelInstance& Scene::getModel(const uint32_t modelID)
{
    return retrieveModel(_models, modelID);
}

const ModelInstance &Scene::getModel(const uint32_t modelID) const
{
    return retrieveModel(_models, modelID);
}

void Scene::removeModel(const uint32_t modelID)
{
    const auto count = _models.erase(modelID);
    if(count == 0)
        throw std::invalid_argument("Could not remove model, ID does not exists");
}

std::vector<uint32_t> Scene::getAllModelIDs() const noexcept
{
    std::vector<uint32_t> result;
    result.reserve(_models.size());
    for(const auto& [modelID, model] : _models)
        result.push_back(modelID);
    return result;
}

uint32_t Scene::addLight(Light::Ptr&& light) noexcept
{
    const auto id = _lightIdFactory++;
    _lights[id] = std::move(light);
    markModified(false);
    return id;
}

const Light& Scene::getLight(const uint32_t lightID) const
{
    const auto it = _lights.find(lightID);
    if(it == _lights.end())
        throw std::invalid_argument("Light with ID " + std::to_string(lightID) + " does not exists");

    return *(it->second);
}

void Scene::removeLight(const uint32_t lightID)
{
    const auto count = _lights.erase(lightID);
    if(count == 0)
        throw std::invalid_argument("Could not remove light, ID does not exists");
}

void Scene::commit()
{
    // Commit models
    std::vector<OSPInstance> instances;
    instances.reserve(_models.size());

    for(auto& [modelID, model] : _models)
    {
        if(model->isVisible())
        {
            model->doCommit();
            instances.push_back(model->handle());
        }
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
    std::vector<OSPLight> lights;
    lights.reserve(_lights.size());

    for(auto& [lightID, light] : _lights)
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
} // namespace brayns
