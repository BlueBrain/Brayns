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

class LayerDistanceAtlas final : public Atlas
{
public:
    static inline const VoxelType type = VoxelType::LayerDistance;

public:
    LayerDistanceAtlas(const brayns::Vector3ui &size, const brayns::Vector3f &spacing, const IDataMangler &dataMangler);

    /**
     * @copydoc Atlas::isValidVoxel(size_t)
     */
    bool isValidVoxel(size_t linealIndex) const noexcept override;

    /**
     * @brief Atlas::getVoxelType()
     */
    VoxelType getVoxelType() const noexcept override;

    /**
     * @brief Returns a voxel value. Passing an out of bounds index will result in undefined behaviour.
     *
     * @param index
     * @return const Vector2f&
     */
    const brayns::Vector2f &operator[](size_t index) const noexcept;

    /**
     * @brief Returns the lowest valid values.
     *
     * @return const brayns::Vector2f&
     */
    const brayns::Vector2f &getLowerLimits() const noexcept;

    /**
     * @brief Returns the highest valid values.
     *
     * @return const brayns::Vector2f&
     */
    const brayns::Vector2f &getHigherLimits() const noexcept;

private:
    std::vector<brayns::Vector2f> _bounds;
    brayns::Vector2f _lowerLimits;
    brayns::Vector2f _higherLimits;
};
