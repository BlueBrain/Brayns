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

#include <api/Atlas.h>
#include <api/DataMangler.h>

class OrientationAtlas final : public Atlas
{
public:
    static inline const VoxelType type = VoxelType::Orientation;

public:
    OrientationAtlas(const brayns::Vector3ui &size, const brayns::Vector3f &spacing, const IDataMangler &dataMangler);

    /**
     * @copydoc Atlas::isValidVoxel(size_t)
     */
    bool isValidVoxel(size_t linealIndex) const noexcept override;

    /**
     * @copydoc Atlas::getVoxelType()
     */
    VoxelType getVoxelType() const noexcept override;

    /**
     * @brief Access the volume quaternions using a lineal index. Passing an out of bounds index results in undefined
     * behaviour.
     *
     * @param index
     * @return const brayns::Quaternion&
     */
    const brayns::Quaternion &operator[](size_t index) const noexcept;

private:
    std::vector<brayns::Quaternion> _voxels;
};
