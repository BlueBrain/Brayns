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

#include "HighlightColumn.h"

#include <brayns/engine/components/GeometryRendererComponent.h>
#include <brayns/engine/geometries/Box.h>

#include <api/usecases/common/ParamsParser.h>

namespace
{
class CoordinatesValidator
{
public:
    static void validate(const HighlighColumParams &params, const AtlasVolume &volume)
    {
        const auto &size = volume.getSize();

        const auto &mainCoords = params.xz_coordinate;
        const auto mainX = mainCoords.x;
        const auto mainZ = mainCoords.y;

        if (size.x <= mainX || size.z <= mainZ)
        {
            throw std::invalid_argument("Column coordinates are out of volume bounds");
        }

        const auto &neighbours = params.neighbours;
        for (const auto &neighbour : neighbours)
        {
            const auto neighbourCoords = neighbour.relative_xz;
            const auto nx = mainX + neighbourCoords.x;
            const auto nz = mainZ + neighbourCoords.y;
            if (size.x <= nx || size.z <= nz)
            {
                throw std::invalid_argument("Neighbour coordinates are out of volume bounds");
            }
        }
    }
};

class ColumnBoundFinder
{
public:
    ColumnBoundFinder(const AtlasVolume &volume)
        : _volume(volume)
        , _data(_volume.getData().asFloats())
    {
    }

    brayns::Box find(const brayns::Vector2ui &xz) const noexcept
    {
        const auto size = _volume.getSize();
        const auto voxelSize = _volume.getVoxelSize();
        const auto frameSize = static_cast<size_t>(size.x * size.y * voxelSize);
        const auto rowWidth = static_cast<size_t>(size.x * voxelSize);
        const auto yLen = static_cast<size_t>(size.y);
        const auto x = static_cast<size_t>(xz.x);
        const auto z = static_cast<size_t>(xz.y);

        brayns::Vector3f min(x, 0, z);
        brayns::Vector3f max(x, yLen - 1, z);
        for (size_t i = 0; i < yLen; ++i)
        {
            const auto y = i;
            const size_t linealIndex = (frameSize * z) + (y * rowWidth) + x;
            const auto valid = _validVoxel(linealIndex);
            if (valid)
            {
                min = brayns::Vector3f(x, y, z);
                break;
            }
        }
        for (size_t i = yLen; i > 0; --i)
        {
            const auto y = i - 1;
            const size_t linealIndex = (frameSize * z) + (y * rowWidth) + x;
            const auto valid = _validVoxel(linealIndex);
            if (valid)
            {
                max = brayns::Vector3f(x, y, z);
                break;
            }
        }

        const auto spacing = _volume.getSpacing();
        return brayns::Box{min * spacing, max * spacing};
    }

private:
    bool _validVoxel(size_t index) const noexcept
    {
        const auto voxelSize = _volume.getVoxelSize();
        const auto end = index + voxelSize;

        size_t zeroes = 0;
        for (size_t i = index; i < end; ++i)
        {
            if (!std::isfinite(_data[i]))
            {
                return false;
            }
            if (_data[i] == 0.f)
            {
                ++zeroes;
            }
        }

        return voxelSize == 1 || zeroes < voxelSize;
    }

private:
    const AtlasVolume &_volume;
    const std::vector<float> _data;
};
}

bool HighlightColumn::isVolumeValid(const AtlasVolume &volume) const
{
    (void)volume;
    return true;
}

void HighlightColumn::execute(const AtlasVolume &volume, const brayns::JsonValue &payload, brayns::Model &model) const
{
    const auto params = ParamsParser::parse<HighlighColumParams>(payload);
    CoordinatesValidator::validate(params, volume);

    const auto numColumns = params.neighbours.size() + 1;
    std::vector<brayns::Box> geometry;
    geometry.reserve(numColumns);
    std::vector<brayns::Vector4f> colors;
    colors.reserve(numColumns);

    ColumnBoundFinder finder(volume);
    const auto coords = params.xz_coordinate;

    geometry.push_back(finder.find(coords));
    colors.push_back(params.color);

    for (const auto &neighbour : params.neighbours)
    {
        const auto nCoords = coords + neighbour.relative_xz;
        geometry.push_back(finder.find(nCoords));
        colors.push_back(neighbour.color);
    }

    auto &component = model.addComponent<brayns::GeometryRendererComponent<brayns::Box>>(std::move(geometry));
    component.setColors(colors);
}
