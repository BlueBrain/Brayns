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

#include "FlatmapVoxels.h"

#include <api/utils/DataUtils.h>

namespace
{
class Extractor
{
public:
    Extractor(const IDataMangler &mangler)
        : _values(mangler.asLongs())
    {
    }

    std::vector<brayns::Vector2l> extractCoordinates()
    {
        auto numCoordinates = _values.size() / 2;

        auto result = std::vector<brayns::Vector2l>();
        result.reserve(numCoordinates);

        for (size_t i = 0; i < numCoordinates; ++i)
        {
            auto index = i * 2;
            result.emplace_back(_values[index], _values[index + 1]);
        }

        return result;
    }

    std::pair<int64_t, int64_t> extractMinMax()
    {
        return DataMinMax::compute(_values);
    }

private:
    std::vector<int64_t> _values;
};
}

FlatmapVoxels::FlatmapVoxels(const IDataMangler &dataMangler)
{
    Extractor extractor(dataMangler);
    _voxels = extractor.extractCoordinates();
    auto minMax = extractor.extractMinMax();
    _min = minMax.first;
    _max = minMax.second;
}

VoxelType FlatmapVoxels::getVoxelType() const noexcept
{
    return type;
}

bool FlatmapVoxels::isValidVoxel(size_t linealIndex) const
{
    assert(linealIndex < _voxels.size());
    auto &element = _voxels[linealIndex];
    return element.x > _min && element.y > _min;
}

int64_t FlatmapVoxels::getMinCoordinate()
{
    return _min;
}

int64_t FlatmapVoxels::getMaxCoordinate()
{
    return _max;
}

const std::vector<brayns::Vector2l> &FlatmapVoxels::getCoordinates() const noexcept
{
    return _voxels;
}
