/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "SceneModelManager.h"

namespace
{
class ModelFinder
{
public:
    template<typename Container>
    static auto findEntryIterator(Container &models, const uint32_t modelId)
    {
        auto begin = models.begin();
        auto end = models.end();
        auto it = std::find_if(
            begin,
            end,
            [mId = modelId](auto &modelEntry)
            {
                auto &model = *modelEntry.model;
                return model.getID() == mId;
            });

        // Shouldn't happen, but...
        if (it == models.end())
        {
            throw std::invalid_argument("No model with id " + std::to_string(modelId) + " was found");
        }

        return it;
    }

    template<typename Container>
    static auto &findEntry(Container &models, const uint32_t modelId)
    {
        return *findEntryIterator(models, modelId);
    }

    template<typename Container>
    static auto findInstanceIterator(Container &instances, const uint32_t instanceId)
    {
        auto begin = instances.begin();
        auto end = instances.end();
        auto it = std::find_if(begin, end, [mId = instanceId](auto &instance) { return instance->getID() == mId; });

        if (it == instances.end())
        {
            throw std::invalid_argument("No instance with id " + std::to_string(instanceId) + " was found");
        }

        return it;
    }

    template<typename Container>
    static auto &findInstance(Container &instances, const uint32_t instanceId)
    {
        return **findInstanceIterator(instances, instanceId);
    }
};
}

namespace brayns
{
ModelInstance &SceneModelManager::addModel(ModelLoadParameters params, std::unique_ptr<Model> model)
{
    std::vector<std::unique_ptr<Model>> models;
    models.push_back(std::move(model));
    auto instances = addModels(std::move(params), std::move(models));
    return *(instances.front());
}

std::vector<ModelInstance *> SceneModelManager::addModels(
    ModelLoadParameters params,
    std::vector<std::unique_ptr<Model>> models)
{
    std::vector<ModelInstance *> result;
    result.reserve(models.size());

    for (auto &model : models)
    {
        auto &entry = _createModelEntry(params, std::move(model));
        auto &instance = _createModelInstance(entry);
        result.push_back(&instance);
    }

    return result;
}

ModelInstance &SceneModelManager::createInstance(const uint32_t instanceID)
{
    auto &sourceInstance = ModelFinder::findInstance(_instances, instanceID);
    auto &model = sourceInstance.getModel();
    auto modelId = model._modelId;
    auto &modelEntry = ModelFinder::findEntry(_models, modelId);
    return _createModelInstance(modelEntry);
}

ModelInstance &SceneModelManager::getModelInstance(const uint32_t modelID)
{
    return ModelFinder::findInstance(_instances, modelID);
}

std::vector<ModelInstance *> &SceneModelManager::getAllModelInstances() noexcept
{
    return _instances;
}

const std::vector<ModelInstance *> &SceneModelManager::getAllModelInstances() const noexcept
{
    return _instances;
}

void SceneModelManager::removeModels(const std::vector<uint32_t> &instanceIDs)
{
    if (instanceIDs.empty())
    {
        return;
    }

    for (auto instanceId : instanceIDs)
    {
        ModelFinder::findInstanceIterator(_instances, instanceId);
    }

    for (auto instanceID : instanceIDs)
    {
        auto it = ModelFinder::findInstanceIterator(_instances, instanceID);
        auto &modelInstance = **it;

        auto &model = modelInstance.getModel();
        auto modelId = model.getID();
        auto modelIt = ModelFinder::findEntryIterator(_models, modelId);
        auto &modelEntry = *modelIt;
        auto &modelInstanceList = modelEntry.instances;
        auto instanceIterator = ModelFinder::findInstanceIterator(modelInstanceList, instanceID);

        modelInstanceList.erase(instanceIterator);
        _instances.erase(it);

        _instanceIdFactory.releaseID(instanceID);

        // If no more instances of the model, get rid of it
        if (modelInstanceList.empty())
        {
            _models.erase(modelIt);
            _modelIdFactory.releaseID(modelId);
        }
    }

    _dirty = true;
}

void SceneModelManager::removeAllModelInstances()
{
    _instanceIdFactory.clear();
    _modelIdFactory.clear();
    _instances.clear();
    _models.clear();
    _dirty = true;
}

const ModelLoadParameters &SceneModelManager::getModelLoadParameters(const uint32_t instanceID) const
{
    auto &modelInstance = ModelFinder::findInstance(_instances, instanceID);
    auto &model = modelInstance.getModel();
    auto modelId = model.getID();
    auto &modelEntry = ModelFinder::findEntry(_models, modelId);
    auto &loadParams = modelEntry.params;

    return loadParams;
}

void SceneModelManager::preRender(const ParametersManager &parameters)
{
    for (auto &entry : _models)
    {
        auto &model = *entry.model;
        model.onPreRender(parameters);
    }
}

bool SceneModelManager::commit()
{
    bool needsRecommit = _dirty;
    _dirty = false;

    for (auto &entry : _models)
    {
        auto &model = *entry.model;
        needsRecommit = model.commit() || needsRecommit;
    }

    for (auto instance : _instances)
    {
        needsRecommit = instance->commit() || needsRecommit;
    }

    return needsRecommit;
}

void SceneModelManager::postRender(const ParametersManager &parameters)
{
    for (auto &entry : _models)
    {
        auto &model = *entry.model;
        model.onPostRender(parameters);
    }
}

Bounds SceneModelManager::getBounds() const noexcept
{
    Bounds result;
    for (const auto instance : _instances)
    {
        const auto &instanceBounds = instance->getBounds();
        result.expand(instanceBounds);
    }

    return result;
}

SceneModelManager::ModelEntry &SceneModelManager::_createModelEntry(
    ModelLoadParameters params,
    std::unique_ptr<Model> model)
{
    auto &modelEntry = _models.emplace_back();
    modelEntry.params = std::move(params);
    modelEntry.model = std::move(model);
    modelEntry.model->_modelId = _modelIdFactory.generateID();
    return modelEntry;
}

ModelInstance &SceneModelManager::_createModelInstance(ModelEntry &modelEntry)
{
    const auto instanceID = _instanceIdFactory.generateID();
    auto &model = *modelEntry.model;
    auto &modelInstanceList = modelEntry.instances;

    auto instance = std::make_unique<ModelInstance>(instanceID, model);
    _instances.push_back(instance.get());
    modelInstanceList.push_back(std::move(instance));

    return *(_instances.back());
}

std::vector<ospray::cpp::Instance> SceneModelManager::getOsprayInstances() noexcept
{
    std::vector<ospray::cpp::Instance> handles;
    handles.reserve(_instances.size());
    for (auto instance : _instances)
    {
        if (!instance->isVisible())
        {
            continue;
        }
        const auto &osprayInstance = instance->getOsprayInstance();
        handles.push_back(osprayInstance);
    }
    return handles;
}
}
