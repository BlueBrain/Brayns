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

#include "OutlineShell.h"

#include <brayns/core/engine/colormethods/SolidColorMethod.h>
#include <brayns/core/engine/components/Geometries.h>
#include <brayns/core/engine/components/GeometryViews.h>
#include <brayns/core/engine/geometry/types/Isosurface.h>
#include <brayns/core/engine/systems/GenericBoundsSystem.h>
#include <brayns/core/engine/systems/GenericColorSystem.h>
#include <brayns/core/engine/systems/GeometryDataSystem.h>
#include <brayns/core/engine/volume/types/RegularVolume.h>

#include <brayns/atlas/api/ModelType.h>

#include <ospray/ospray_cpp/ext/rkcommon.h>

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
                result[i] = std::numeric_limits<uint8_t>::max();
            }
        }
        return result;
    }
};

class VolumeBuilder
{
public:
    static brayns::Volume fromAtlas(const Atlas &atlas)
    {
        brayns::RegularVolume grid;
        grid.dataType = brayns::VolumeDataType::UnsignedChar;
        grid.voxels = ValidVoxelGridFilter::filter(atlas);
        grid.size = atlas.getSize();
        grid.spacing = atlas.getSpacing();
        return brayns::Volume(grid);
    }
};
}

std::string OutlineShell::getName() const
{
    return "Outline mesh shell";
}

bool OutlineShell::isValidAtlas(const Atlas &atlas) const
{
    (void)atlas;
    return true;
}

std::shared_ptr<brayns::Model> OutlineShell::run(const Atlas &atlas, const brayns::JsonValue &payload) const
{
    (void)payload;

    auto model = std::make_shared<brayns::Model>(ModelType::atlas);

    auto isoVolume = VolumeBuilder::fromAtlas(atlas);
    auto isoValues = std::vector<float>{1.f};
    auto isoSurface = brayns::Isosurface{std::move(isoVolume), std::move(isoValues)};

    auto &components = model->getComponents();
    auto &geometries = components.add<brayns::Geometries>();
    auto &geometry = geometries.elements.emplace_back(std::move(isoSurface));
    auto &views = components.add<brayns::GeometryViews>();
    auto &view = views.elements.emplace_back(geometry);
    view.setColor(brayns::Vector4f(1.f, 1.f, 1.f, 0.3f));

    auto &systems = model->getSystems();
    systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
    systems.setDataSystem<brayns::GeometryDataSystem>();

    auto colorMethods = std::vector<std::unique_ptr<brayns::IColorMethod>>();
    colorMethods.push_back(std::make_unique<brayns::SolidColorMethod>());
    systems.setColorSystem<brayns::GenericColorSystem>(std::move(colorMethods));

    return model;
}
