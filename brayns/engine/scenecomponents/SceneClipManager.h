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
#include <brayns/engine/Model.h>
#include <brayns/engine/ModelInstance.h>

#include <memory>
#include <unordered_map>

namespace brayns
{
/**
 * @brief Manages all the models used to clip renderable objects in the scene
 */
class SceneClipManager
{
public:
    /**
     * @brief Adds a new clipping model
     *
     * @param clippingModel
     * @return uint32_t
     */
    uint32_t addClippingModel(std::unique_ptr<Model> clippingModel) noexcept;

    /**
     * @brief Removes a clipping model object
     *
     * @param id
     */
    void removeClippingModel(const uint32_t id);

    /**
     * @brief Removes all clipping models from this manager
     */
    void removeAllClippingModels() noexcept;

private:
    friend class Scene;

    struct ClippingEntry
    {
        std::unique_ptr<Model> model;
        std::unique_ptr<ModelInstance> instance;
    };

    /**
     * @brief Will commit any not synced data with OSPRay
     *
     * @return true if there was anything to commit, false otherwise
     */
    bool commit();

    /**
     * @brief Return the handles of the instances of the clipping geometry
     * @return std::vector<OSPInstance>
     */
    std::vector<OSPInstance> getInstanceHandles() noexcept;

private:
    bool _dirty{false};
    IDFactory<uint32_t> _idFactory;
    std::unordered_map<uint32_t, ClippingEntry> _clippingModels;
};
}
