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

#include <brayns/atlas/api/Atlas.h>
#include <brayns/atlas/api/DataMangler.h>

/**
 * @brief Represents a scalar value atlas volume of any kind.
 */
class ScalarAtlas final : public Atlas
{
public:
    static inline const VoxelType type = VoxelType::Scalar;

public:
    ScalarAtlas(const brayns::Vector3ui &size, const brayns::Vector3f &spacing, const IDataMangler &dataMangler);

    /**
     * @copydoc Atlas::isValidVoxel(size_t)
     */
    bool isValidVoxel(size_t linealIndex) const noexcept override;

    /**
     * @copydoc Atlas::getVoxelType()
     */
    VoxelType getVoxelType() const noexcept override;

    /**
     * @brief Returns the lowest scalar value in the volume.
     *
     * @return double
     */
    double getMinValue() const noexcept;

    /**
     * @brief Returns the highest scalar value in the volume.
     *
     * @return double
     */
    double getMaxValue() const noexcept;

    /**
     * @brief Access the volume data using a lineal index. Passing an out of bounds index results in undefined
     * behaviour.
     *
     * @param index
     * @return double
     */
    double operator[](size_t index) const noexcept;

    /**
     * @brief Returns the volume voxels.
     *
     * @return const std::vector<double>&
     */
    const std::vector<double> &getValues() const noexcept;

private:
    std::vector<double> _data;
    double _min = 0.;
    double _max = 0.;
};
