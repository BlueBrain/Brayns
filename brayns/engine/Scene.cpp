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
Scene::Scene()
 : _bounds(Vector3f(0.f), Vector3f(0.f))
{
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

bool Scene::commit()
{
    bool needsCommit = false;
    if (!_handle)
    {
        needsCommit = true;
        _handle = ospNewWorld();
    }

    // Commit models
    if (_modelManager.commit() || _clippingManager.commit())
    {
        std::vector<OSPInstance> instances;
        auto modelInstances = _modelManager.getInstanceHandles();
        instances.insert(instances.end(), modelInstances.begin(), modelInstances.end());
        auto clipInstances = _clippingManager.getInstanceHandles();
        instances.insert(instances.end(), clipInstances.begin(), clipInstances.end());

        if(!instances.empty())
        {
            commitHandleList(instances, OSPDataType::OSP_INSTANCE, _handle, "instance");
            needsCommit = true;
        }
    }

    if (_lightManager.commit())
    {
        auto lights = _lightManager.getLightHandles();
        if(!lights.empty())
        {
            commitHandleList(lights, OSPDataType::OSP_LIGHT, _handle, "light");
            needsCommit = true;
        }
    }

    // Commit handle
    if(needsCommit)
    {
        ospCommit(_handle);
    }

    return needsCommit;
}

SceneModelManager &Scene::getModelManager() noexcept
{
    return _modelManager;
}

const SceneModelManager &Scene::getModelManager() const noexcept
{
    return _modelManager;
}

SceneClipManager &Scene::getClipManager() noexcept
{
    return _clippingManager;
}

SceneLightManager &Scene::getLightManager() noexcept
{
    return _lightManager;
}

OSPWorld Scene::handle() const noexcept
{
    return _handle;
}

uint64_t Scene::_getSizeBytes() const noexcept
{
    uint64_t baseSize = sizeof(Scene);

    baseSize += _modelManager.getSizeInBytes();
    baseSize += _clippingManager.getSizeInBytes();
    baseSize += _lightManager.getSizeInBytes();

    return baseSize;
}
} // namespace brayns
