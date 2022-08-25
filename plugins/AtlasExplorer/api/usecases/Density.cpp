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

#include "Density.h"

#include <brayns/engine/common/ExtractComponent.h>
#include <brayns/engine/components/VolumeRendererComponent.h>
#include <brayns/engine/volume/types/RegularVolume.h>

#include <api/usecases/common/DataUtils.h>

namespace
{
struct DensityVolumeInfo
{
    std::vector<uint8_t> data;
    brayns::VolumeDataType dataType;
    brayns::Vector2f minMax;
};

class DensityVolumeBuilder
{
public:
    static DensityVolumeInfo build(const AtlasVolume &volume)
    {
        const auto &data = volume.getData();
        const auto isSigned = data.isTypeSigned();
        if (isSigned)
        {
            return _processSignedData(data);
        }

        return _processUnsignedData(data);
    }

private:
    static DensityVolumeInfo _processSignedData(const IDataMangler &data)
    {
        const auto typeSize = data.getTypeSize();
        if (typeSize <= 2)
        {
            return _buildProcessedData(brayns::VolumeDataType::Short, data.asShorts());
        }

        if (typeSize <= 4)
        {
            return _buildProcessedData(brayns::VolumeDataType::Float, data.asFloats());
        }

        return _buildProcessedData(brayns::VolumeDataType::Double, data.asDoubles());
    }

    static DensityVolumeInfo _processUnsignedData(const IDataMangler &data)
    {
        const auto typeSize = data.getTypeSize();
        if (typeSize == 1)
        {
            return _buildProcessedData(brayns::VolumeDataType::UnsignedChar, data.asBytes());
        }
        if (typeSize == 2)
        {
            return _buildProcessedData(brayns::VolumeDataType::UnsignedShort, data.asUnsingedShorts());
        }
        if (typeSize <= 4)
        {
            return _buildProcessedData(brayns::VolumeDataType::Float, data.asFloats());
        }

        return _buildProcessedData(brayns::VolumeDataType::Double, data.asDoubles());
    }

    template<typename T>
    static DensityVolumeInfo _buildProcessedData(brayns::VolumeDataType type, const std::vector<T> &values)
    {
        const auto minMax = DataMinMax::compute(values);

        DensityVolumeInfo result;
        result.dataType = type;
        result.data = DataToBytes::convert(values);
        result.minMax = _castMinMax(minMax);
        return result;
    }

    template<typename T>
    static brayns::Vector2f _castMinMax(const std::pair<T, T> &input)
    {
        return {static_cast<float>(input.first), static_cast<float>(input.second)};
    }
};
}

std::string Density::getName() const
{
    return "Density";
}

bool Density::isVolumeValid(const AtlasVolume &volume) const
{
    return volume.getVoxelSize() == 1;
}

std::unique_ptr<brayns::Model> Density::execute(const AtlasVolume &volume, const brayns::JsonValue &payload) const
{
    (void)payload;

    auto densityData = DensityVolumeBuilder::build(volume);

    brayns::RegularVolume densityVolume;
    densityVolume.voxels = std::move(densityData.data);
    densityVolume.dataType = densityData.dataType;
    densityVolume.size = volume.getSize();
    densityVolume.spacing = volume.getSpacing();

    auto model = std::make_unique<brayns::Model>();
    model->addComponent<brayns::VolumeRendererComponent>(brayns::Volume(std::move(densityVolume)));

    auto &colorRamp = brayns::ExtractComponent::colorRamp(*model);
    colorRamp.setValuesRange(densityData.minMax);

    return model;
}
