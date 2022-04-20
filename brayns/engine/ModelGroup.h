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

#include <ospray/ospray.h>

#include <vector>

namespace brayns
{
/**
 * @brief The ModelGroup class encapsulates the handling of an OSPGroup object
 */
class ModelGroup
{
public:
    /**
     * @brief Initializes the OSPGroup handle
     */
    ModelGroup();

    /**
     * @brief Releases the OSPGroup handle
     */
    ~ModelGroup();

    ModelGroup(const ModelGroup &) = delete;
    ModelGroup &operator=(const ModelGroup &) = delete;

    ModelGroup(ModelGroup &&) noexcept;
    ModelGroup &operator=(ModelGroup &&) noexcept;

    /**
     * @brief Adds a new geometric model and marks the group as dirty so it gets committed
     */
    void addGeometricModel(OSPGeometricModel model);

    /**
     * @brief Removes an existing geometric model and mark the group as dirty so it gets committed
     */
    void removeGeometricModel(OSPGeometricModel model);

    /**
     * @brief Adds a new volumetric model and marks the group as dirty so it gets committed
     */
    void addVolumetricModel(OSPVolumetricModel model);

    /**
     * @brief Removes an existing volumetric model and mark the group as dirty so it gets committed
     */
    void removeVolumetricModel(OSPVolumetricModel model);

    /**
     * @brief Adds a new geometric clipping model and marks the group as dirty so it gets commited
     */
    void addClippingModel(OSPGeometricModel model);

    /**
     * @brief Removes an existing clipping model and mark the group as dirty so it gets committed
     */
    void removeClippingModel(OSPGeometricModel model);

    /**
     * @brief Returns the OSPGroup handle
     */
    OSPGroup handle() const noexcept;

private:
    friend class Model;

    /**
     * @brief Synchronizes the group data with OSPRay
     */
    void commit();

private:
    OSPGroup _handle{nullptr};
    std::vector<OSPGeometricModel> _geometryModels;
    std::vector<OSPVolumetricModel> _volumeModels;
    std::vector<OSPGeometricModel> _clippingModels;
    bool _modified{false};
};
}
