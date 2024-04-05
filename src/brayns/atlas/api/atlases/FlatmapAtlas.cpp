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

#include "FlatmapAtlas.h"

#include <brayns/atlas/api/utils/DataUtils.h>

#include <cassert>

namespace
{
class Extractor
{
public:
    explicit Extractor(const IDataMangler &mangler):
        _values(mangler.asLongs())
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

FlatmapAtlas::FlatmapAtlas(const brayns::Vector3ui &size, const brayns::Vector3f &spacing, const IDataMangler &data):
    Atlas(size, spacing)
{
    Extractor extractor(data);
    _voxels = extractor.extractCoordinates();
    auto minMax = extractor.extractMinMax();
    _min = minMax.first;
}

bool FlatmapAtlas::isValidVoxel(size_t index) const noexcept
{
    assert(_isValidIndex(index));
    auto &element = _voxels[index];
    return element.x > _min && element.y > _min;
}

VoxelType FlatmapAtlas::getVoxelType() const noexcept
{
    return type;
}

const brayns::Vector2l &FlatmapAtlas::operator[](size_t index) const noexcept
{
    assert(_isValidIndex(index));
    return _voxels[index];
}
