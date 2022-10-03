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
#pragma omp parallel for
        for (size_t i = 0; i < data.size(); ++i)
        {
            auto isEmpty = data[i] == minValue;

            if (!std::isfinite(data[i]) || isEmpty)
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
#pragma omp parallel for
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
    static brayns::RegularVolume extract(const AtlasData &volume)
    {
        auto &data = *volume.data;
        auto doubles = data.asDoubles();
        auto voxelSize = volume.voxelSize;

        brayns::RegularVolume result;
        result.dataType = brayns::VolumeDataType::UnsignedChar;
        result.voxels = ValidVoxelGridFilter::filter(voxelSize, doubles);
        result.size = volume.size;
        result.spacing = volume.spacing;
        return result;
    }
};
}

std::string OutlineShell::getName() const
{
    return "Outline mesh shell";
}

bool OutlineShell::isVolumeValid(const AtlasData &volume) const
{
    (void)volume;
    return true;
}

std::unique_ptr<brayns::Model> OutlineShell::execute(const AtlasData &volume, const brayns::JsonValue &payload) const
{
    (void)payload;

    auto model = std::make_unique<brayns::Model>();

    auto isoVolume = brayns::Volume(FeaturesExtractor::extract(volume));
    auto isoValues = std::vector<float>{255.f};
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
