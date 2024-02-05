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

#pragma once

#include <brayns/common/BaseObject.h>
#include <brayns/engine/Light.h>

#include <map>
#include <unordered_map>

namespace brayns
{
class LightManager : public BaseObject
{
public:
    /**
     * @brief addLight Attaches a light source to the scene
     * @param light Object representing the light source
     */
    size_t addLight(LightPtr light);

    /**
     * @brief removeLight Removes a light source from the scene for a given id
     * @param id ID of Light source to be removed
     */
    void removeLight(const size_t id);

    /**
     * @brief removeLight Removes a light source from the scene for a given
     * light
     * @param light Light source to be removed
     */
    void removeLight(LightPtr light);

    /**
     * @brief getLight Gets a light source from the scene for a given id.
     * Note: If changing the light then call markModified to propagate the
     * changes.
     * @param id Id of the light
     * @return Pointer to the light, nullptr if not found
     */
    LightPtr getLight(const size_t id);

    /**
     * @brief getLights Gets the lights
     * @return Immutable list of all light sources and their id
     */
    const std::map<size_t, LightPtr>& getLights() const;

    /**
     * @brief clearLights Removes all light sources
     */
    void clearLights();

private:
    std::map<size_t, LightPtr> _lights;
    std::unordered_map<LightPtr, size_t> _lightsInverse;
    size_t _IDctr{0};
};
} // namespace brayns
