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

#include "VectorAtlas.h"

#include <cassert>

namespace
{
float finiteOrZero(float value)
{
    if (std::isfinite(value))
    {
        return value;
    }

    return 0.0F;
}

brayns::Vector3f sanitizeVector(brayns::Vector3f vector)
{
    for (auto i = std::size_t(0); i < std::size_t(3); ++i)
    {
        vector[i] = finiteOrZero(vector[i]);
    }

    auto norm = brayns::math::length(vector);

    if (norm == 0.0F)
    {
        return vector;
    }

    return vector / norm;
}

std::vector<brayns::Vector3f> extractVectors(const IDataMangler &data)
{
    auto floats = data.asFloats();
    auto itemCount = floats.size() / 3;

    auto result = std::vector<brayns::Vector3f>(itemCount);

#pragma omp parallel for
    for (size_t i = 0; i < itemCount; ++i)
    {
        auto index = i * 3;

        auto x = floats[index];
        auto y = floats[index + 1];
        auto z = floats[index + 2];

        result[i] = sanitizeVector({x, y, z});
    }

    return result;
}
}

VectorAtlas::VectorAtlas(
    const brayns::Vector3ui &size,
    const brayns::Vector3f &spacing,
    const IDataMangler &dataMangler):
    Atlas(size, spacing),
    _voxels(extractVectors(dataMangler))
{
}

bool VectorAtlas::isValidVoxel(size_t index) const noexcept
{
    assert(_isValidIndex(index));
    return brayns::math::dot(_voxels[index], _voxels[index]) > 0.f;
}

VoxelType VectorAtlas::getVoxelType() const noexcept
{
    return type;
}

const brayns::Vector3f &VectorAtlas::operator[](size_t index) const noexcept
{
    assert(_isValidIndex(index));
    return _voxels[index];
}
