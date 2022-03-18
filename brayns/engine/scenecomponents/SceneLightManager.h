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

#include <brayns/common/IDFactory.h>
#include <brayns/engine/Light.h>

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
     * @brief Removes a light identifid by its ID
     * @throws std::invalid_argument if the light id does not exist
     * @param lightId
     */
    void removeLight(const uint32_t lightId);

    /**
     * @brief Removes all lights from the scene
     */
    void removeAllLigts() noexcept;

private:
    friend class Scene;

    /**
     * @brief Will commit any not synced data with OSPRay.
     *
     * @return true if there was anything to commit, false otherwise
     */
    bool commit();

    /**
     * @brief Get the OSPRay handles of all lights
     *
     * @return std::vector<OSPLight>
     */
    std::vector<OSPLight> getLightHandles() const noexcept;

    /**
     * @brief returns the size in bytes of this light manager
     * @return size_t
     */
    size_t getSizeInBytes() const noexcept;

private:
    bool _dirty;
    IDFactory<uint32_t> _idFactory;
    std::unordered_map<uint32_t, std::unique_ptr<Light>> _lights;
};
}
