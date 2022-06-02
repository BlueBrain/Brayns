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
    {
    }

    brayns::Box find(const brayns::Vector2ui &xz) const noexcept
    {
        const auto &size = _volume.getSize();
        const auto &spacing = _volume.getSpacing();
        const auto x = static_cast<float>(xz.x);
        const auto z = static_cast<float>(xz.y);
        const auto y = static_cast<float>(size.y);

        brayns::Vector3f min(x, -1.f, z);
        brayns::Vector3f max(x + 1.f, y, z + 1.f);

        return brayns::Box{min * spacing, max * spacing};
    }

private:
    const AtlasVolume &_volume;
};
}

bool HighlightColumn::isVolumeValid(const AtlasVolume &volume) const
{
    (void)volume;
    return true;
}

std::unique_ptr<brayns::Model> HighlightColumn::execute(const AtlasVolume &volume, const brayns::JsonValue &payload)
    const
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

    auto model = std::make_unique<brayns::Model>();

    auto &component = model->addComponent<brayns::GeometryRendererComponent<brayns::Box>>(std::move(geometry));
    component.setColors(colors);

    return model;
}
