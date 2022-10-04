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

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/geometry/types/Isosurface.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryCommitSystem.h>
#include <brayns/engine/systems/GeometryInitSystem.h>
#include <brayns/engine/volume/types/RegularVolume.h>

namespace
{
class ValidVoxelGridFilter
{
public:
    static std::vector<uint8_t> filter(const Atlas &atlas)
    {
        auto voxelCount = atlas.getVoxelCount();
        auto result = std::vector<uint8_t>(voxelCount, 0u);

        for (size_t i = 0; i < voxelCount; ++i)
        {
            if (atlas.isValidVoxel(i))
            {
                result[i] = 255u;
            }
        }
    }
};

class FeaturesExtractor
{
public:
    static brayns::RegularVolume extract(const Atlas &atlas)
    {
        brayns::RegularVolume result;
        result.dataType = brayns::VolumeDataType::UnsignedChar;
        result.voxels = ValidVoxelGridFilter::filter(atlas);
        result.size = atlas.getSize();
        return result;
    }
};
}

std::string OutlineShell::getName() const
{
    return "Outline mesh shell";
}

bool OutlineShell::isVolumeValid(const Atlas &volume) const
{
    (void)volume;
    return true;
}

std::unique_ptr<brayns::Model> OutlineShell::run(const Atlas &volume, const brayns::JsonValue &payload) const
{
    (void)payload;

    auto model = std::make_unique<brayns::Model>();

    auto isoVolume = brayns::Volume(FeaturesExtractor::extract(volume));
    auto isoValues = std::vector<float>{1.f};
    auto isoSurface = brayns::Isosurface{std::move(isoVolume), std::move(isoValues)};

    auto &components = model->getComponents();
    auto &geometries = components.add<brayns::Geometries>();
    geometries.elements.emplace_back(std::move(isoSurface));

    auto &systems = model->getSystems();
    systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
    systems.setInitSystem<brayns::GeometryInitSystem>();
    systems.setCommitSystem<brayns::GeometryCommitSystem>();

    return model;
}
