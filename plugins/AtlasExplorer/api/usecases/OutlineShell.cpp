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

#include "OutlineShell.h"

#include <brayns/engine/components/IsosurfaceRendererComponent.h>
#include <brayns/engine/components/VolumeRendererComponent.h>
#include <brayns/engine/volumes/RegularVolume.h>

#include <api/usecases/common/DataUtils.h>

namespace
{
class ValidVoxelGridFilter
{
public:
    static std::vector<uint8_t> filter(size_t voxelSize, const std::vector<double> &data)
    {
        if (voxelSize == 1)
        {
            return _filterScalar(data);
        }

        return _filterNDimensional(voxelSize, data);
    }

private:
    static std::vector<uint8_t> _filterScalar(const std::vector<double> &data)
    {
        const auto minMax = DataMinMax::compute(data);
        const auto minValue = minMax.first;

        std::vector<uint8_t> result(data.size(), 0u);
        for (size_t i = 0; i < data.size(); ++i)
        {
            if (!std::isfinite(data[i]) || data[i] == minValue)
            {
                continue;
            }

            result[i] = 255;
        }

        return result;
    }

    static std::vector<uint8_t> _filterNDimensional(size_t voxelSize, const std::vector<double> &data)
    {
        std::vector<uint8_t> result(data.size() / voxelSize, 0u);
        for (size_t i = 0; i < result.size(); ++i)
        {
            size_t zeroCount = 0;
            bool validElement = true;
            size_t index = i * voxelSize;
            for (size_t j = 0; j < voxelSize; ++j)
            {
                if (!std::isfinite(data[index + j]))
                {
                    validElement = false;
                    break;
                }
                if (data[index + j] == 0.0)
                {
                    ++zeroCount;
                }
            }
            if (zeroCount == voxelSize || !validElement)
            {
                continue;
            }

            result[i] = 255;
        }
        return result;
    }
};

class FeaturesExtractor
{
public:
    static brayns::RegularVolume extract(const AtlasVolume &volume)
    {
        const auto &data = volume.getData();
        const auto doubles = data.asDoubles();
        const auto voxelSize = volume.getVoxelSize();

        brayns::RegularVolume result;
        result.dataType = brayns::VolumeDataType::UNSIGNED_CHAR;
        result.data = ValidVoxelGridFilter::filter(voxelSize, doubles);
        result.size = volume.getSize();
        result.spacing = volume.getSpacing();
        return result;
    }
};
}

bool OutlineShell::isVolumeValid(const AtlasVolume &volume) const
{
    (void)volume;
    return true;
}

void OutlineShell::execute(const AtlasVolume &volume, const brayns::JsonValue &payload, brayns::Model &model) const
{
    (void)payload;
    brayns::RegularVolume shellVolume = FeaturesExtractor::extract(volume);

    // model.addComponent<brayns::VolumeRendererComponent<brayns::RegularVolume>>(std::move(shellVolume));
    model.addComponent<brayns::IsosurfaceRendererComponent<brayns::RegularVolume>>(std::move(shellVolume), 1.f);
}
