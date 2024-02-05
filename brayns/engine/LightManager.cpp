/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
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

#include <algorithm>

namespace brayns
{
size_t LightManager::addLight(LightPtr light)
{
    // If light already added, return id
    auto itInv = _lightsInverse.find(light);
    if (itInv != _lightsInverse.end())
    {
        markModified();
        return itInv->second;
    }

    // If lights are empty we reset id counter to avoid huge numbers
    if (_lights.empty())
        _IDctr = 0;

    const size_t id = _IDctr++;
    _lights.insert({id, light});
    _lightsInverse.insert({light, id});
    markModified();
    return id;
}

void LightManager::removeLight(const size_t id)
{
    auto it = _lights.find(id);
    if (it != _lights.end())
    {
        auto light = it->second;

        auto itInv = _lightsInverse.find(light);
        assert(itInv != _lightsInverse.end());
        if (itInv != _lightsInverse.end())
            _lightsInverse.erase(itInv);

        _lights.erase(it);

        markModified();
    }
}

void LightManager::removeLight(LightPtr light)
{
    auto itInv = _lightsInverse.find(light);

    if (itInv != _lightsInverse.end())
    {
        const size_t id = itInv->second;
        auto it = _lights.find(id);
        assert(it != _lights.end());
        if (it != _lights.end())
            _lights.erase(it);
    }
}

LightPtr LightManager::getLight(const size_t id)
{
    auto it = _lights.find(id);
    if (it != _lights.end())
        return it->second;

    return nullptr;
}

const std::map<size_t, LightPtr>& LightManager::getLights() const
{
    return _lights;
}

void LightManager::clearLights()
{
    _lights.clear();
    _lightsInverse.clear();
    markModified();
}

} // namespace brayns
