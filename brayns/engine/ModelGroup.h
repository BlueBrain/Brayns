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

#include "GeometryObject.h"
#include "VolumeObject.h"

#include <ospray/ospray_cpp/Group.h>

#include <vector>

namespace brayns
{
/**
 * @brief The ModelGroup class encapsulates the handling of an OSPGroup object
 */
class ModelGroup
{
public:
    ModelGroup(const ModelGroup &) = delete;
    ModelGroup &operator=(const ModelGroup &) = delete;

    ModelGroup(ModelGroup &&) = delete;
    ModelGroup &operator=(ModelGroup &&) = delete;

    /**
     * @brief Adds a new geometric model and marks the group as dirty so it gets committed
     */
    void addGeometricModel(const GeometryObject &model);

    /**
     * @brief Removes an existing geometric model and mark the group as dirty so it gets committed
     */
    void removeGeometricModel(const GeometryObject &model);

    /**
     * @brief Adds a new volumetric model and marks the group as dirty so it gets committed
     */
    void addVolumetricModel(const VolumeObject &model);

    /**
     * @brief Removes an existing volumetric model and mark the group as dirty so it gets committed
     */
    void removeVolumetricModel(const VolumeObject &model);

    /**
     * @brief Adds a new geometric clipping model and marks the group as dirty so it gets commited
     */
    void addClippingModel(const GeometryObject &model);

    /**
     * @brief Removes an existing clipping model and mark the group as dirty so it gets committed
     */
    void removeClippingModel(const GeometryObject &model);

private:
    friend class Model;
    friend class ModelInstance;

    /**
     * @brief Returns the OSPGroup handle
     */
    ospray::cpp::Group &getOsprayGroup() noexcept;

    /**
     * @brief Synchronizes the group data with OSPRay
     */
    bool commit();

private:
    ospray::cpp::Group _osprayGroup;
    std::vector<ospray::cpp::GeometricModel> _geometryModels;
    std::vector<ospray::cpp::VolumetricModel> _volumeModels;
    std::vector<ospray::cpp::GeometricModel> _clippingModels;
    bool _modified{false};
};
}
