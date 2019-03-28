/* Copyright (c) 2019, EPFL/Blue Brain Project
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

#include <brayns/common/light/Light.h>
#include <brayns/common/utils/utils.h>

#include <algorithm>

namespace brayns
{
size_t LightManager::addLight(LightPtr light)
{
    removeLight(light);
    const size_t id = _IDctr++;
    _lights.insert({id, light});
    markModified();
    return id;
}

void LightManager::removeLight(const size_t id)
{
    auto it = _lights.find(id);
    if (it != _lights.end())
    {
        _lights.erase(it);
        markModified();
    }
}

void LightManager::removeLight(LightPtr light)
{
    for (auto itr = _lights.begin(); itr != _lights.end(); ++itr)
    {
        if (itr->second == light)
        {
            markModified();
            _lights.erase(itr);
            break;
        }
    }

    // erase_value(_lights, light);
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
    markModified();
}

} // namespace brayns
