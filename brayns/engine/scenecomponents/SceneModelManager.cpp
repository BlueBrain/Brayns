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
decltype(auto) findInstanceIterator(Container &&models, const uint32_t id)
{
    auto it = std::find(models.begin(), models.end(), [mId = id](auto &model) { return model->getID() == mId; });

    if (it == models.end())
    {
        throw std::invalid_argument("No Model with id " + std::to_string(id) + " was found");
    }

    return it;
}

template<typename Container>
decltype(auto) findInstance(Container &&models, const uint32_t id)
{
    auto it = findInstanceIterator(models, id);
    return *(*it);
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

    return _createModelInstance(modelEntry);
}

ModelInstance &SceneModelManager::createInstance(const uint32_t modelID)
{
    auto &sourceInstance = findInstance(_modelInstances, modelID);
    auto &model = sourceInstance.getModel();
    auto modelIndex = model._modelIndex;
    auto &modelEntry = _models[modelIndex];
    return _createModelInstance(modelEntry);
}

ModelInstance &SceneModelManager::getModelInstance(const uint32_t modelID)
{
    return findInstance(_modelInstances, modelID);
}

const std::vector<ModelInstance *> SceneModelManager::getAllModelInstances() const noexcept
{
    return _modelInstances;
}

void SceneModelManager::removeModel(const uint32_t modelID)
{
    auto it = findInstanceIterator(_modelInstances, modelID);
    auto &modelInstance = *it;

    auto &model = modelInstance->getModel();
    auto modelIndex = model._modelIndex;
    auto &modelEntry = _models[modelIndex];
    auto &modelInstanceList = modelEntry.instances;
    auto instanceIterator = findInstanceIterator(modelInstanceList, modelID);
    modelInstanceList.erase(instanceIterator);

    _modelInstances.erase(it);
}

const ModelLoadParameters SceneModelManager::getModelLoadParameters(const uint32_t modelID) const
{
    auto &modelInstance = findInstance(_modelInstances, modelID);
    auto &model = modelInstance.getModel();
    auto modelIndex = model._modelIndex;
    auto &modelEntry = _models[modelIndex];
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
    for (const auto &instance : _modelInstances)
    {
        const auto &instanceBounds = instance->getBounds();
        result.expand(instanceBounds);
    }

    return result;
}

ModelInstance &SceneModelManager::_createModelInstance(ModelEntry &modelEntry)
{
    const auto instanceID = _idFactory.requestID();
    auto &model = *modelEntry.model;
    auto &modelInstance = modelEntry.instances;

    auto instance = std::make_unique<ModelInstance>(instanceID, model);
    _modelInstances.push_back(instance.get());
    modelInstance.push_back(std::move(instance));

    return *(_modelInstances.back());
}

std::vector<OSPInstance> SceneModelManager::getInstanceHandles() noexcept
{
    std::vector<OSPInstance> handles;
    handles.reserve(_modelInstances.size());
    for (auto &instance : _modelInstances)
    {
        auto handle = instance->handle();
        handles.push_back(handle);
    }
    return handles;
}
}
