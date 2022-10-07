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

#include "OrientationAtlas.h"

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

            result[i] = glm::normalize(brayns::Quaternion(w, x, y, z));
        }
        return result;
    }

private:
    static bool _validQuaternion(const brayns::Vector4f &test)
    {
        return glm::length2(test) > 0.f && glm::compMin(glm::isfinite(test));
    }
};
}

OrientationAtlas::OrientationAtlas(
    const brayns::Vector3ui &size,
    const brayns::Vector3f &spacing,
    const IDataMangler &dataMangler)
    : Atlas(size, spacing)
    , _voxels(QuaternionExtractor::extract(dataMangler))
{
}

bool OrientationAtlas::isValidVoxel(size_t linealIndex) const
{
    _checkIndex(linealIndex);
    return glm::length2(_voxels[linealIndex]) > 0.f;
}

const brayns::Quaternion &OrientationAtlas::operator[](size_t index) const noexcept
{
    return _voxels[index];
}

const brayns::Quaternion &OrientationAtlas::at(size_t index) const
{
    _checkIndex(index);
    return _voxels[index];
}

VoxelType OrientationAtlas::getVoxelType() const noexcept
{
    return type;
}