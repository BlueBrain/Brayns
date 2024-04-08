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

#include "OrientationAtlas.h"

#include <cassert>

namespace
{
class QuaternionExtractor
{
public:
    static std::vector<brayns::Quaternion> extract(const IDataMangler &data)
    {
        auto floats = data.asFloats();
        auto numQuaternions = floats.size() / 4;

        auto result = std::vector<brayns::Quaternion>(numQuaternions, brayns::Quaternion(0.f, 0.f, 0.f, 0.f));

        for (size_t i = 0; i < numQuaternions; ++i)
        {
            auto index = i * 4;
            auto w = floats[index];
            auto x = floats[index + 1];
            auto y = floats[index + 2];
            auto z = floats[index + 3];

            if (!_validQuaternion({w, x, y, z}))
            {
                continue;
            }

            result[i] = brayns::math::normalize(brayns::Quaternion(w, x, y, z));
        }
        return result;
    }

private:
    static bool _validQuaternion(const brayns::Vector4f &test)
    {
        return brayns::math::dot(test, test) > 0.f && brayns::math::reduce_min(brayns::math::isfinite(test));
    }
};
}

OrientationAtlas::OrientationAtlas(
    const brayns::Vector3ui &size,
    const brayns::Vector3f &spacing,
    const IDataMangler &dataMangler):
    Atlas(size, spacing),
    _voxels(QuaternionExtractor::extract(dataMangler))
{
}

bool OrientationAtlas::isValidVoxel(size_t index) const noexcept
{
    assert(_isValidIndex(index));
    return brayns::math::dot(_voxels[index], _voxels[index]) > 0.f;
}

VoxelType OrientationAtlas::getVoxelType() const noexcept
{
    return type;
}

const brayns::Quaternion &OrientationAtlas::operator[](size_t index) const noexcept
{
    assert(_isValidIndex(index));
    return _voxels[index];
}
