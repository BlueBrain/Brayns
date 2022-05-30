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

#include "OrientationVolume.h"

#include <api/usecases/OutlineShell.h>
#include <components/RotationVolumeComponent.h>

namespace
{
class QuaternionsToVoxelGrid
{
public:
    static std::vector<uint8_t> convert(const std::vector<brayns::Quaternion> &quaternions)
    {
        std::vector<uint8_t> result(quaternions.size(), 0u);

        for (size_t i = 0; i < quaternions.size(); ++i)
        {
            size_t zeroCount = 0;
            for (size_t j = 0; j < 4; ++j)
            {
                if (quaternions[i][j] == 0.f)
                {
                    ++zeroCount;
                }
            }
            if (zeroCount == 4)
            {
                continue;
            }

            result[i] = 255;
        }
        return result;
    }
};
}

OrientationVolume::OrientationVolume(
    const brayns::Vector3ui &size,
    const brayns::Vector3f &spacing,
    std::vector<brayns::Quaternion> data)
    : _gridSize(size)
    , _gridSpacing(spacing)
    , _data(std::move(data))
{
}

void OrientationVolume::handleUseCase(VisualizationUseCase useCase, brayns::Model &model) const
{
    if (useCase == VisualizationUseCase::OUTLINE_SHELL)
    {
        OutlineShellData data;
        data.data = QuaternionsToVoxelGrid::convert(_data);
        data.gridSpacing = _gridSpacing;
        data.gridSize = _gridSize;
        OutlineShell::generate(model, std::move(data));
        return;
    }

    if (useCase == VisualizationUseCase::ORIENTATION_FIELD)
    {
        model.addComponent<RotationVolumeComponent>(_gridSize, _gridSpacing, _data);
        return;
    }

    throw std::runtime_error("Orientation volume cannot handle " + brayns::enumToString(useCase));
}
