/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/engine/components/Bounds.h>

#include "VoxelType.h"

/**
 * @brief Abstract representation of an atlas volume.
 */
class Atlas
{
public:
    Atlas(const brayns::Vector3ui &size, const brayns::Vector3f &spacing);
    virtual ~Atlas() = default;

    /**
     * @brief Get the volume grid dimensions.
     * @return const brayns::Vector3ui&
     */
    const brayns::Vector3ui &getSize() const noexcept;

    /**
     * @brief Get the cell dimensions.
     * @return const brayns::Vector3f&
     */
    const brayns::Vector3f &getSpacing() const noexcept;

    /**
     * @brief Get the number of voxels (width * height * depth)
     * @return size_t
     */
    size_t getVoxelCount() const noexcept;

    /**
     * @brief Computes a voxel bounds. Passing out of bounds coordinates results in undefined behaviour.
     * @param coordinates Cartesian coordinates of the voxel.
     * @return Bounds The bounds of the voxel.
     */
    brayns::Bounds getVoxelBounds(const brayns::Vector3ui &coordinates) const noexcept;

    /**
     * @brief Computes a voxel bounds from a lineal index. Passing an out of bounds index results in undefined
     * behaviour.
     * @param linealIndex Lineal index of the voxel.
     * @return Bounds The bounds of the voxel.
     */
    brayns::Bounds getVoxelBounds(size_t linealIndex) const noexcept;

    /**
     * @brief Returns wether the pointed voxel contains a valid value. Passing out of bounds coordinates results in
     * undefined behaviour.
     * @param coordinates Cartesian coordinates of the voxel.
     * @return true if the voxel value is not empty and the value is usable.
     */
    bool isValidVoxel(const brayns::Vector3ui &coordinates) const noexcept;

    /**
     * @brief Returns wether the pointed voxel contains a valid value. Passing an out of bounds index results in
     * undefined behaviour.
     * @param linealIndex flattened voxels index.
     * @return true if the voxel value is not empty and the value is usable.
     */
    virtual bool isValidVoxel(size_t linealIndex) const noexcept = 0;

    /**
     * @brief Returns the type of voxel of this Atlas volume.
     * @return VoxelType type of voxel.
     */
    virtual VoxelType getVoxelType() const noexcept = 0;

protected:
    /**
     * @brief Returns true if a given index is within the volume bounds.
     *
     * @param index
     * @return true
     * @return false
     */
    bool _isValidIndex(size_t index) const noexcept;

private:
    brayns::Vector3ui _size;
    brayns::Vector3f _spacing;
};
