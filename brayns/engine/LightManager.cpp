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
void LightManager::addLight(LightPtr light)
{
    removeLight(light);
    _lights.push_back(light);
    markModified();
}

void LightManager::removeLight(const size_t id)
{
    erase_if(_lights, [id](const auto& light) {
        return light->getId() == static_cast<int>(id);
    });
    markModified();
}

void LightManager::removeLight(LightPtr light)
{
    erase_value(_lights, light);
    markModified();
}

LightPtr LightManager::getLight(const size_t id)
{
    for (auto& light : _lights)
        if (light->getId() == static_cast<int>(id))
            return light;
    return nullptr;
}

const std::vector<LightPtr>& LightManager::getLights() const
{
    return _lights;
}

void LightManager::clearLights()
{
    _lights.clear();
    markModified();
}

LightPtr LightManager::addDirectionalLight(const Vector3f& direction,
                                           const Vector3f& color,
                                           float intensity)
{
    auto light = std::make_shared<Light>(LightType::DIRECTIONAL, color,
                                         intensity, _IDctr++);
    light->setProperty({"direction", toArray<3, double>(direction)});
    addLight(light);
    return light;
}

LightPtr LightManager::addSphereLight(const Vector3f& position, float radius,
                                      const Vector3f& color, float intensity)
{
    auto light =
        std::make_shared<Light>(LightType::SPHERE, color, intensity, _IDctr++);
    light->setProperty({"position", toArray<3, double>(position)});
    light->setProperty({"radius", static_cast<double>(radius)});
    addLight(light);
    return light;
}

LightPtr LightManager::addQuadLight(const Vector3f& position,
                                    const Vector3f& edge1,
                                    const Vector3f& edge2,
                                    const Vector3f& color, float intensity)
{
    auto light =
        std::make_shared<Light>(LightType::QUAD, color, intensity, _IDctr++);
    light->setProperty({"position", toArray<3, double>(position)});
    light->setProperty({"edge1", toArray<3, double>(edge1)});
    light->setProperty({"edge2", toArray<3, double>(edge2)});
    addLight(light);
    return light;
}

LightPtr LightManager::addSpotLight(const Vector3f& position,
                                    const Vector3f& direction,
                                    const float openingAngle,
                                    const float penumbraAngle,
                                    const float radius, const Vector3f& color,
                                    float intensity)
{
    auto light = std::make_shared<Light>(LightType::SPOTLIGHT, color, intensity,
                                         _IDctr++);
    light->setProperty({"position", toArray<3, double>(position)});
    light->setProperty({"direction", toArray<3, double>(direction)});
    light->setProperty({"openingAngle", static_cast<double>(openingAngle)});
    light->setProperty({"penumbraAngle", static_cast<double>(penumbraAngle)});
    light->setProperty({"radius", static_cast<double>(radius)});
    addLight(light);
    return light;
}

} // namespace brayns
