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

#include "LightManager.h"

namespace brayns
{
uint32_t LightManager::addLight(Light light) noexcept
{
    const auto id = _idFactory.generateID();
    _lights.emplace(id, std::move(light));
    _dirty = true;
    return id;
}

void LightManager::removeLights(const std::vector<uint32_t> &lightIds)
{
    for (auto lightId : lightIds)
    {
        if (_lights.find(lightId) == _lights.end())
        {
            throw std::invalid_argument("No light with id " + std::to_string(lightId) + " was found");
        }
    }
    for (auto lightId : lightIds)
    {
        _lights.erase(lightId);
        _idFactory.releaseID(lightId);
    }
    _dirty = true;
}

void LightManager::removeAllLights() noexcept
{
    _idFactory.clear();
    _lights.clear();
    _dirty = true;
}

Bounds LightManager::getBounds() const noexcept
{
    Bounds bounds;
    for (const auto &[lightId, light] : _lights)
    {
        bounds.expand(light.computeBounds(Matrix4f(1.f)));
    }
    return bounds;
}

bool LightManager::commit()
{
    auto result = _dirty;
    _dirty = false;
    return result;
}

std::vector<ospray::cpp::Light> LightManager::getHandles() const noexcept
{
    std::vector<ospray::cpp::Light> handles;
    handles.reserve(_lights.size());
    for (auto &[lightId, light] : _lights)
    {
        auto &osprayLight = light.getHandle();
        handles.push_back(osprayLight);
    }

    return handles;
}
}
