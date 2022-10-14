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

#include <brayns/engine/components/Bounds.h>

#include "VoxelType.h"

/**
 * @brief Abstract representation of an atlas volume.
 */
class Atlas
{
public:
    Atlas(const brayns::Vector3ui &size, const brayns::Vector3f &spacing);

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
     * @brief Computes a voxel bounds.
     * @param coordinates Cartesian coordinates of the voxel.
     * @return Bounds The bounds of the voxel.
     */
    brayns::Bounds getVoxelBounds(const brayns::Vector3ui &coordinates) const;

    /**
     * @brief Computes a voxel bounds from a lineal index.
     * @param linealIndex Lineal index of the voxel.
     * @return Bounds The bounds of the voxel.
     */
    brayns::Bounds getVoxelBounds(size_t linealIndex) const;

    /**
     * @brief Returns wether the pointed voxel contains a valid value.
     * @param coordinates Cartesian coordinates of the voxel.
     * @return true if the voxel value is not empty and the value is usable.
     */
    bool isValidVoxel(const brayns::Vector3ui &coordinates) const;

    /**
     * @brief Returns wether the pointed voxel contains a valid value.
     * @param linealIndex flattened voxels index.
     * @return true if the voxel value is not empty and the value is usable.
     */
    virtual bool isValidVoxel(size_t linealIndex) const = 0;

    /**
     * @brief Returns the type of voxel of this Atlas volume.
     * @return VoxelType type of voxel.
     */
    virtual VoxelType getVoxelType() const noexcept = 0;

protected:
    /**
     * @brief Checks if the given lineal index is within the volume bounds.
     * @param linealIndex
     */
    void _checkIndex(size_t linealIndex) const;

private:
    brayns::Vector3ui _size;
    brayns::Vector3f _spacing;
};
