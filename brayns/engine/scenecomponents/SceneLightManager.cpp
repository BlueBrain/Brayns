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

#include "SceneLightManager.h"

namespace brayns
{
uint32_t SceneLightManager::addLight(std::unique_ptr<Light> light) noexcept
{
    const auto id = _idFactory.requestID();
    light->commit();
    _lights[id] = std::move(light);
    _dirty = true;
    return id;
}

void SceneLightManager::removeLight(const uint32_t lightId)
{
    const auto eraseCount = _lights.erase(lightId);
    if (eraseCount == 0)
    {
        throw std::invalid_argument("No light with id " + std::to_string(lightId) + " was found");
    }
    _dirty = true;
}

void SceneLightManager::removeAllLigts() noexcept
{
    for (const auto &[lightId, light] : _lights)
    {
        _idFactory.releaseID(lightId);
    }

    _lights.clear();
    _dirty = true;
}

bool SceneLightManager::commit()
{
    auto result = _dirty;
    _dirty = false;
    return result;
}

std::vector<OSPLight> SceneLightManager::getLightHandles() const noexcept
{
    std::vector<OSPLight> handles;
    handles.reserve(_lights.size());
    for (const auto &[lightId, light] : _lights)
    {
        const auto handle = light->handle();
        handles.push_back(handle);
    }

    return handles;
}

size_t SceneLightManager::getSizeInBytes() const noexcept
{
    size_t size = 0;
    for (const auto &[lightID, light] : _lights)
    {
        size += sizeof(lightID) + light->getSizeInBytes();
    }

    return size;
}
}
