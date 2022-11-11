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

#include "ModelManager.h"

namespace
{
class InstanceFinder
{
public:
    template<typename Container>
    static auto findIterator(Container &instances, uint32_t instanceId)
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
    static auto &find(Container &instances, uint32_t instanceId)
    {
        return **findIterator(instances, instanceId);
    }
};
}

namespace brayns
{
ModelInstance *ModelManager::add(std::shared_ptr<Model> model)
{
    model->init();
    auto instanceId = _instanceIdFactory.generateID();
    auto instance = std::make_unique<ModelInstance>(instanceId, std::move(model));
    _instances.push_back(std::move(instance));
    _dirty = true;
    return _instances.back().get();
}

std::vector<ModelInstance *> ModelManager::add(std::vector<std::shared_ptr<Model>> models)
{
    std::vector<ModelInstance *> result;
    result.reserve(models.size());

    for (auto &model : models)
    {
        result.push_back(add(std::move(model)));
    }

    return result;
}

ModelInstance *ModelManager::createInstance(const uint32_t instanceID)
{
    auto &sourceInstance = InstanceFinder::find(_instances, instanceID);
    auto instanceId = _instanceIdFactory.generateID();
    _instances.push_back(std::make_unique<ModelInstance>(instanceId, sourceInstance));
    _dirty = true;
    return _instances.back().get();
}

ModelInstance &ModelManager::getModelInstance(const uint32_t modelID)
{
    return InstanceFinder::find(_instances, modelID);
}

const std::vector<std::unique_ptr<ModelInstance>> &ModelManager::getAllModelInstances() const noexcept
{
    return _instances;
}

void ModelManager::removeModelInstancesById(const std::vector<uint32_t> &instanceIDs)
{
    if (instanceIDs.empty())
    {
        return;
    }

    for (auto instanceId : instanceIDs)
    {
        InstanceFinder::findIterator(_instances, instanceId);
    }

    _removeModelInstances(instanceIDs);
}

void ModelManager::removeAllModelInstances()
{
    _instanceIdFactory.clear();
    _instances.clear();
    _dirty = true;
}

void ModelManager::update(const ParametersManager &parameters)
{
    for (auto &instance : _instances)
    {
        auto &model = instance->getModel();
        auto view = model.getSystemsView();
        view.update(parameters);
    }
}

CommitResult ModelManager::commit()
{
    auto result = CommitResult{std::exchange(_dirty, false)};

    for (auto &instance : _instances)
    {
        auto &model = instance->getModel();
        auto view = model.getSystemsView();
        auto modelResult = view.commit();
        result.needsRebuildBVH |= modelResult.needsRebuildBVH;
        result.needsRender |= modelResult.needsRender;

        auto instanceResult = instance->commit();
        result.needsRebuildBVH |= instanceResult;
    }

    return result;
}

Bounds ModelManager::getBounds() const noexcept
{
    Bounds result;

    for (auto &instance : _instances)
    {
        result.expand(instance->getBounds());
    }

    return result;
}

std::vector<ospray::cpp::Instance> ModelManager::getHandles() noexcept
{
    std::vector<ospray::cpp::Instance> handles;
    handles.reserve(_instances.size());

    for (auto &instance : _instances)
    {
        if (!instance->isVisible())
        {
            continue;
        }
        handles.push_back(instance->getHandle());
    }

    return handles;
}

void ModelManager::_removeModelInstances(const std::vector<uint32_t> &ids)
{
    for (auto id : ids)
    {
        auto it = InstanceFinder::findIterator(_instances, id);
        _instances.erase(it);
        _instanceIdFactory.releaseID(id);
    }

    _dirty = true;
}
}
