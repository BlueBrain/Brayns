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
template<typename Container>
decltype(auto) findModelIterator(Container &&models, const uint32_t modelId)
{
    auto begin = models.begin();
    auto end = models.end();
    auto it = std::find_if(begin, end, [mId = modelId](auto &modelEntry)
    {
        auto &model = *modelEntry.model;
        return model.getID() == mId;
    });

    // Shouldn't happen, but...
    if(it == models.end())
    {
        throw std::invalid_argument("No model with id " + std::to_string(modelId) + " was found");
    }

    return it;
}

template<typename Container>
decltype(auto) findInstanceIterator(Container &&instances, const uint32_t instanceId)
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
}

namespace brayns
{
ModelInstance &SceneModelManager::addModel(ModelLoadParameters params, std::unique_ptr<Model> model)
{
    _models.emplace_back();
    auto &modelEntry = _models.back();
    modelEntry.params = std::move(params);
    modelEntry.model = std::move(model);
    modelEntry.model->_modelId = _modelIdFactory.requestID();

    return _createModelInstance(modelEntry);
}

ModelInstance &SceneModelManager::createInstance(const uint32_t instanceID)
{
    auto &sourceInstance = **findInstanceIterator(_instances, instanceID);
    auto &model = sourceInstance.getModel();
    auto modelId = model._modelId;
    auto &modelEntry = *findModelIterator(_models, modelId);
    return _createModelInstance(modelEntry);
}

ModelInstance &SceneModelManager::getModelInstance(const uint32_t modelID)
{
    return **findInstanceIterator(_instances, modelID);
}

std::vector<ModelInstance *> &SceneModelManager::getAllModelInstances() noexcept
{
    return _instances;
}

const std::vector<ModelInstance *> &SceneModelManager::getAllModelInstances() const noexcept
{
    return _instances;
}

void SceneModelManager::removeModel(const uint32_t instanceID)
{
    auto it = findInstanceIterator(_instances, instanceID);
    auto &modelInstance = **it;

    auto &model = modelInstance.getModel();
    auto modelId = model.getID();
    auto modelIt = findModelIterator(_models, modelId);
    auto &modelEntry = *modelIt;
    auto &modelInstanceList = modelEntry.instances;
    auto instanceIterator = findInstanceIterator(modelInstanceList, instanceID);

    modelInstanceList.erase(instanceIterator);
    _instances.erase(it);

    _instanceIdFactory.releaseID(instanceID);

    // If no more instances of the model, get rid of it
    if(modelInstanceList.empty())
    {
        _models.erase(modelIt);
        _modelIdFactory.releaseID(modelId);
    }
}

const ModelLoadParameters &SceneModelManager::getModelLoadParameters(const uint32_t instanceID) const
{
    auto &modelInstance = **findInstanceIterator(_instances, instanceID);
    auto &model = modelInstance.getModel();
    auto modelId = model.getID();
    auto &modelEntry = *findModelIterator(_models, modelId);
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
    bool needsRecommit = false;
    for (auto &entry : _models)
    {
        auto &model = *entry.model;
        auto &instances = entry.instances;

        bool modelChanged = model.commit();

        bool instancesChanged = false;
        for (auto &instance : instances)
        {
            if(!instance->isVisible())
            {
                continue;
            }

            instancesChanged = instancesChanged || instance->commit(modelChanged);
        }

        needsRecommit = needsRecommit || instancesChanged;
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

ModelInstance &SceneModelManager::_createModelInstance(ModelEntry &modelEntry)
{
    const auto instanceID = _instanceIdFactory.requestID();
    auto &model = *modelEntry.model;
    auto &modelInstanceList = modelEntry.instances;

    auto instance = std::make_unique<ModelInstance>(instanceID, model);
    _instances.push_back(instance.get());
    modelInstanceList.push_back(std::move(instance));

    return *(_instances.back());
}

std::vector<OSPInstance> SceneModelManager::getInstanceHandles() noexcept
{
    std::vector<OSPInstance> handles;
    handles.reserve(_instances.size());
    for (auto instance : _instances)
    {
        if(!instance->isVisible())
        {
            continue;
        }
        auto handle = instance->handle();
        handles.push_back(handle);
    }
    return handles;
}

size_t SceneModelManager::getSizeInBytes() const noexcept
{
    size_t size = 0;
    for (const auto &modelEntry : _models)
    {
        size += sizeof(ModelEntry);
        size += modelEntry.model->getSizeInBytes();
        size += sizeof(ModelInstance) * modelEntry.instances.size();
    }

    size += _instances.size() * sizeof(ModelInstance*);

    return size;
}
}
