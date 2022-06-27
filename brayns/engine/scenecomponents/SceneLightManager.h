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

#pragma once

#include <brayns/engine/Light.h>
#include <brayns/utils/IDFactory.h>

#include <memory>
#include <unordered_map>

namespace brayns
{
/**
 * @brief Handles the lights of the scene
 */
class SceneLightManager
{
public:
    /**
     * @brief Adds a new light and returns its ID
     *
     * @param light
     * @return uint32_t
     */
    uint32_t addLight(std::unique_ptr<Light> light) noexcept;

    /**
     * @brief Removes a list of lights identifid by their ID
     * @param lightIds
     * @throws std::invalid_argument if any of the light id does not belong to any existing light
     */
    void removeLights(const std::vector<uint32_t> &lightId);

    /**
     * @brief Removes all lights from the scene
     */
    void removeAllLights() noexcept;

    /**
     * @brief Return the bounds of all the lights in the manager
     * @return Bounds
     */
    Bounds getBounds() const noexcept;

private:
    friend class Scene;

    /**
     * @brief Will commit any not synced data with Ospray.
     *
     * @return true if there was anything to commit, false otherwise
     */
    bool commit();

    /**
     * @brief Get the Ospray handles of all lights
     *
     * @return std::vector<ospray::cpp::Light>
     */
    std::vector<ospray::cpp::Light> getOsprayLights() const noexcept;

private:
    bool _dirty{false};
    IDFactory<uint32_t> _idFactory;
    std::unordered_map<uint32_t, std::unique_ptr<Light>> _lights;
};
}
