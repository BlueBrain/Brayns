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

#include "Density.h"

#include <brayns/engine/components/ColorRamp.h>
#include <brayns/engine/components/Volumes.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/VolumeDataSystem.h>
#include <brayns/engine/volume/types/RegularVolume.h>

#include <api/ModelType.h>
#include <api/atlases/ScalarAtlas.h>
#include <api/utils/DataUtils.h>

std::string Density::getName() const
{
    return "Density";
}

bool Density::isValidAtlas(const Atlas &atlas) const
{
    return atlas.getVoxelType() == VoxelType::Scalar;
}

std::shared_ptr<brayns::Model> Density::run(const Atlas &atlas, const brayns::JsonValue &payload) const
{
    (void)payload;

    assert(dynamic_cast<const ScalarAtlas *>(&atlas));
    auto &scalarAtlas = static_cast<const ScalarAtlas &>(atlas);

    brayns::RegularVolume densityVolume;
    densityVolume.voxels = DataToBytes::convert(scalarAtlas.getValues());
    densityVolume.dataType = brayns::VolumeDataType::Double;
    densityVolume.size = scalarAtlas.getSize();
    densityVolume.spacing = scalarAtlas.getSpacing();

    auto model = std::make_shared<brayns::Model>(ModelType::atlas);

    auto &components = model->getComponents();

    auto &volumes = components.add<brayns::Volumes>();
    volumes.elements.emplace_back(std::move(densityVolume));

    auto &colorRamp = components.add<brayns::ColorRamp>();
    colorRamp.setValuesRange(
        {static_cast<float>(scalarAtlas.getMinValue()), static_cast<float>(scalarAtlas.getMaxValue())});

    auto &systems = model->getSystems();
    systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Volumes>>();
    systems.setDataSystem<brayns::VolumeDataSystem>();

    return model;
}
