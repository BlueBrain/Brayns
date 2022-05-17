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

#include "RotationVolumeComponent.h"

#include <cmath>

namespace
{
class ValidRotationChecker
{
public:
    static bool isValid(const brayns::Quaternion &quaternion)
    {
        const auto xValid = std::isfinite(quaternion.x);
        const auto yValid = std::isfinite(quaternion.y);
        const auto zValid = std::isfinite(quaternion.z);
        const auto wValid = std::isfinite(quaternion.w);
        return xValid && yValid && zValid && wValid;
    }

    static size_t count(const std::vector<brayns::Quaternion> &rotations)
    {
        size_t result = 0;
        for (const auto &q : rotations)
        {
            if (isValid(q))
            {
                ++result;
            }
        }
        return result;
    }
};

class RotatedAxisVolumeBuilder
{
public:
    struct RotatedAxisVolume
    {
        std::vector<brayns::Primitive> xAxis;
        std::vector<brayns::Primitive> yAxis;
        std::vector<brayns::Primitive> zAxis;
    };

    RotatedAxisVolume build(
        const brayns::Vector3ui &sizes,
        const brayns::Vector3f &dimensions,
        const std::vector<brayns::Quaternion> &rotations)
    {
        const auto validElementCount = ValidRotationChecker::count(rotations);

        const auto width = sizes.x;
        const auto height = sizes.y;
        const auto depth = sizes.z;
        const auto frameSize = width * height;

        const auto linealSize = width * height * depth;

        auto result = RotatedAxisVolume();
        auto &xAxis = result.xAxis;
        xAxis.reserve(validElementCount);
        auto &yAxis = result.yAxis;
        yAxis.reserve(validElementCount);
        auto &zAxis = result.zAxis;
        zAxis.reserve(validElementCount);

        for (size_t i = 0; i < linealSize; ++i)
        {
            const auto &quaternion = rotations[i];
            if (!ValidRotationChecker::isValid(quaternion))
            {
                continue;
            }
        }

        return result;
    }

private:
    static brayns::Primitive _buildAxis(const brayns::Vector3f &dimensions, )
};
}

RotationVolumeComponent::RotationVolumeComponent(
    const brayns::Vector3ui &sizes,
    const brayns::Vector3f &dimensions,
    std::vector<brayns::Quaternion> &rotations)
{
    if (glm::compMul(sizes) != rotations.size())
    {
        throw std::invalid_argument("Size and rotation count is different");
    }
}

brayns::Bounds RotationVolumeComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
}

void RotationVolumeComponent::onCreate()
{
}

bool RotationVolumeComponent::commit()
{
}

void RotationVolumeComponent::onDestroy()
{
}
