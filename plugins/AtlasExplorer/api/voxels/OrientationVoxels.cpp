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

#include "OrientationVoxels.h"

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

            result[i] = brayns::Quaternion(w, x, y, z);
        }
        return result;
    }

private:
    static bool _validQuaternion(const brayns::Vector4f &test)
    {
        if (glm::length(test) == 0.f)
        {
            return false;
        }
        if (!glm::compMin(glm::isfinite(test)))
        {
            return false;
        }
        return true;
    }
};
}

OrientationVoxels::OrientationVoxels(const IDataMangler &dataMangler)
    : _voxels(QuaternionExtractor::extract(dataMangler))
{
}

bool OrientationVoxels::isValidVoxel(size_t linealIndex) const
{
    assert(linealIndex < _voxels.size());
    return glm::length2(_voxels[linealIndex]) > 0.f;
}

const std::vector<brayns::Quaternion> &OrientationVoxels::getQuaternions() const noexcept
{
    return _voxels;
}
