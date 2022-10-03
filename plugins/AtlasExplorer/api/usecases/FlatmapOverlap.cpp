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

#include "FlatmapOverlap.h"

#include <io/nrrdloader/VolumeReader.h>

#include "common/DataUtils.h"

namespace
{
class ValidVoxels
{
public:
    static std::vector<size_t> getIndexList(const std::vector<int64_t> &data)
    {
        auto minMax = DataMinMax::compute(data);
        std::cout << minMax.first << ", " << minMax.second << std::endl;

        auto numVoxels = data.size() / 2;
        std::vector<size_t> indices;
        indices.reserve(numVoxels);

        for (size_t i = 0; i < numVoxels; ++i)
        {
            if (!_filterVoxel(data, i))
            {
                continue;
            }
            indices.push_back(i);
        }
        return indices;
    }

private:
    static bool _filterVoxel(const std::vector<int64_t> &, size_t)
    {
        // auto globalIndex = index * 2;
        // auto coords = brayns::Vector2ui(data[globalIndex], data[globalIndex + 1]);

        return true;
        // return glm::compAdd(coords)
    }
};
/*
class OverlappingAreas
{
public:
    static std::vector<size_t> getIndexList(const std::vector<uint64_t> &flatmap)
    {
    }

private:
    using OverlapMap = std::unordered_map<uint64_t, std::unordered_map<uint64_t, size_t>>;

    static OverlapMap _buildOverlapMap(const std::vector<uint64_t> &flatmap)
};*/
}

std::string FlatmapOverlap::getName() const
{
    return "Flatmap overlapping areas";
}

bool FlatmapOverlap::isVolumeValid(const AtlasData &volume) const
{
    return volume.voxelSize == 2;
}

std::unique_ptr<brayns::Model> FlatmapOverlap::execute(const AtlasData &volume, const brayns::JsonValue &payload) const
{
    (void)payload;
    auto coordinateData = volume.data->asLongs();
    auto validIndices = ValidVoxels::getIndexList(coordinateData);
    return {};
}
