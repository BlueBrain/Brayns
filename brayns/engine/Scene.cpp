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

        if (!instances.empty())
        {
            auto instanceBuffer = DataHandler::copyBuffer(instances, OSPDataType::OSP_INSTANCE);
            ospSetParam(_handle, "instance", OSPDataType::OSP_DATA, &instanceBuffer.handle);
            needsCommit = true;
        }
    }

    if (_lightManager.commit())
    {
        auto lights = _lightManager.getLightHandles();
        if (!lights.empty())
        {
            auto lightBuffer = DataHandler::copyBuffer(lights, OSPDataType::OSP_LIGHT);
            ospSetParam(_handle, "light", OSPDataType::OSP_DATA, &lightBuffer.handle);
            needsCommit = true;
        }
    }

    // Commit handle
    if (needsCommit)
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
} // namespace brayns
