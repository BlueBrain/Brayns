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
auto &retrieveModel(ModelMap &models, const uint32_t modelID)
{
    auto it = models.find(modelID);
    if (it == models.end())
        throw std::invalid_argument("Model with ID " + std::to_string(modelID) + " does not exists");

    return *(it->second);
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
    _bounds = Bounds();
    for (auto &[modelID, instance] : _modelInstances)
    {
        const auto &modelBounds = instance->getBounds();
        _bounds.expand(modelBounds);
    }
}

ModelInstance &Scene::addModel(ModelLoadParameters params, Model::Ptr &&model)
{
    if (!model)
        throw std::invalid_argument("Scene: Attempted to add a null model");

    // Create a new model entry
    const auto modelIndex = _models.size();
    _models.emplace_back();
    auto &modelEntry = _models.back();

    // Store load parameters on the model entry
    modelEntry.params = std::move(params);

    // Set the model index and store it on the model entry
    model->_modelIndex = modelIndex;
    auto modelPtr = model.get();
    modelEntry.model = std::move(model);

    // Create an instance that we will use for rendering
    auto &modelInstance = _createModelInstance(modelPtr);
    auto modelID = modelInstance.getID();

    // Add the model instance to the list of the loaded model instances
    modelEntry.instances.insert(modelID);

    markModified(false);

    return modelInstance;
}

ModelInstance &Scene::createInstance(const uint32_t modelID)
{
    auto &modelInstance = retrieveModel(_modelInstances, modelID);
    auto &model = modelInstance.getModel();
    auto modelIndex = model._modelIndex;

    auto &newInstance = _createModelInstance(&model);
    auto newInstanceID = newInstance.getID();

    auto &modelEntry = _models[modelIndex];
    modelEntry.instances.insert(newInstanceID);

    markModified(false);
    return newInstance;
}

ModelInstance &Scene::getModelInstance(const uint32_t modelID)
{
    return retrieveModel(_modelInstances, modelID);
}

void Scene::removeModel(const uint32_t modelID)
{
    // Get the source model index
    auto &instance = getModelInstance(modelID);
    auto &model = instance.getModel();
    auto modelIndex = model._modelIndex;

    // Remove the model instance
    _modelInstances.erase(modelID);

    // Remove the instance from the list of instances of the given model
    auto &modelEntry = _models[modelIndex];
    auto &instanceList = modelEntry.instances;
    instanceList.erase(modelID);

    // If no more instances refer to the model, remove it
    if (instanceList.empty())
    {
        auto it = _models.begin();
        std::advance(it, modelIndex);
        _models.erase(it);
    }
}

std::vector<const ModelInstance *> Scene::getAllModelInstances() const noexcept
{
    std::vector<const ModelInstance *> result;
    result.reserve(_models.size());
    for (const auto &[modelID, model] : _modelInstances)
        result.push_back(model.get());
    return result;
}

uint32_t Scene::addClippingModel(Model::Ptr &&clippingModel)
{
    _clippingModels[_clippingModelIDFactory] = std::move(clippingModel);
    return _clippingModelIDFactory++;
}

Model &Scene::getClippingModel(const uint32_t modelID)
{
    auto it = _clippingModels.find(modelID);
    if (it == _clippingModels.end())
    {
        throw std::invalid_argument("No clipping model with ID" + std::to_string(modelID) + " found");
    }

    return *(it->second);
}

std::map<uint32_t, Model::Ptr> &Scene::getAllClippingModels() noexcept
{
    return _clippingModels;
}

void Scene::removeClippingModel(const uint32_t clippingModelID)
{
    const auto count = _clippingModels.erase(clippingModelID);

    if (count == 0)
        throw std::invalid_argument("Clippoing model with ID " + std::to_string(clippingModelID) + " does not exists");
}

uint32_t Scene::addLight(Light::Ptr &&light) noexcept
{
    const auto id = _lightIdFactory++;
    _lights[id] = std::move(light);
    markModified(false);
    return id;
}

std::map<uint32_t, Light::Ptr> &Scene::getAllLights() noexcept
{
    return _lights;
}

Light &Scene::getLight(const uint32_t lightID)
{
    const auto it = _lights.find(lightID);
    if (it == _lights.end())
        throw std::invalid_argument("Light with ID " + std::to_string(lightID) + " does not exists");

    return *(it->second);
}

void Scene::removeLight(const uint32_t lightID)
{
    const auto count = _lights.erase(lightID);
    if (count == 0)
        throw std::invalid_argument("Could not remove light, ID does not exists");
    markModified(false);
}

void Scene::removeAllLights() noexcept
{
    _lights.clear();
    markModified(false);
}

void Scene::preRender(const ParametersManager &params)
{
    for (auto &[modelID, modelInstance] : _modelInstances)
    {
        auto &model = modelInstance->getModel();
        model.onPreRender(params);
    }
}

void Scene::postRender(const ParametersManager &params)
{
    for (auto &[modelID, modelInstance] : _modelInstances)
    {
        auto &model = modelInstance->getModel();
        model.onPostRender(params);
    }
}

void Scene::commit()
{
    if (!_handle)
        _handle = ospNewWorld();

    // Commit models
    std::vector<OSPInstance> instances;
    instances.reserve(_models.size());

    for (auto &[modelID, model] : _modelInstances)
    {
        if (model->isVisible())
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
