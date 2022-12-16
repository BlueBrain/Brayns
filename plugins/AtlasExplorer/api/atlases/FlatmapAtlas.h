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

#include <api/Atlas.h>
#include <api/DataMangler.h>

class FlatmapAtlas final : public Atlas
{
public:
    inline static const VoxelType type = VoxelType::Flatmap;

public:
    FlatmapAtlas(const brayns::Vector3ui &size, const brayns::Vector3f &spacing, const IDataMangler &dataMangler);

    /**
     * @copydoc Atlas::isValidVoxel(size_t)
     */
    bool isValidVoxel(size_t linealIndex) const noexcept override;

    /**
     * @copydoc Atlas::getVoxelType()
     */
    VoxelType getVoxelType() const noexcept override;

    /**
     * @brief Access the flatmap coordinates of the volume using a lineal index. Passing an out of bounds index results
     * in undefined behaviour.
     *
     * @param index
     * @return const brayns::Vector2l&
     */
    const brayns::Vector2l &operator[](size_t index) const noexcept;

private:
    std::vector<brayns::Vector2l> _voxels;
    int64_t _min;
};
