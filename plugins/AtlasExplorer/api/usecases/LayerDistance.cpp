/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "LayerDistance.h"

#include "common/ParamsParser.h"

#include <api/ModelType.h>
#include <api/atlases/LayerDistanceAtlas.h>

#include <brayns/engine/components/ColorRamp.h>
#include <brayns/engine/components/Volumes.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/VolumeDataSystem.h>
#include <brayns/engine/volume/types/RegularVolume.h>

namespace
{
class LayerDistanceModelFactory
{
public:
    static std::shared_ptr<brayns::Model> create(const LayerDistanceAtlas &atlas, LayerDistanceType type)
    {
        if (type == LayerDistanceType::lower)
        {
            return _create(atlas, 0);
        }

        return _create(atlas, 1);
    }

private:
    static std::shared_ptr<brayns::Model> _create(const LayerDistanceAtlas &atlas, std::size_t index)
    {
        _checkValidData(atlas, index);

        auto model = std::make_shared<brayns::Model>(ModelType::atlas);

        auto volume = brayns::RegularVolume();
        volume.dataType = brayns::VolumeDataType::Float;
        volume.size = atlas.getSize();
        volume.spacing = atlas.getSpacing();
        volume.voxels = _createVoxels(atlas, index);

        auto colorRamp = _createColorRamp(atlas, index);

        auto &components = model->getComponents();
        components.add<brayns::Volumes>(std::move(volume));
        components.add<brayns::ColorRamp>(std::move(colorRamp));

        auto &systems = model->getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Volumes>>();
        systems.setDataSystem<brayns::VolumeDataSystem>();

        return model;
    }

    static std::vector<uint8_t> _createVoxels(const LayerDistanceAtlas &atlas, std::size_t index)
    {
        auto size = atlas.getVoxelCount();
        auto values = std::vector<uint8_t>(size * sizeof(float));

        for (size_t i = 0; i < size; ++i)
        {
            auto value = atlas[i][index];
            auto pos = i * sizeof(float);
            std::memcpy(&values[pos], &value, sizeof(float));
        }

        return values;
    }

    static brayns::ColorRamp _createColorRamp(const LayerDistanceAtlas &atlas, std::size_t index)
    {
        auto colors = std::vector<brayns::Vector4f>{
            brayns::Vector4f(1.f, 0.f, 0.f, 1.f),
            brayns::Vector4f(1.f, 0.5f, 0.f, 1.f),
            brayns::Vector4f(1.f, 1.f, 0.f, 1.f),
            brayns::Vector4f(0.8f, 0.8f, 0.8f, 1.f)};

        auto min = atlas.getLowerLimits()[index];
        auto max = atlas.getHigherLimits()[index];

        auto colorRamp = brayns::ColorRamp();
        colorRamp.setColors(std::move(colors));
        colorRamp.setValuesRange({min, max});
        return colorRamp;
    }

    static void _checkValidData(const LayerDistanceAtlas &atlas, std::size_t index)
    {
        auto min = atlas.getLowerLimits()[index];
        auto max = atlas.getHigherLimits()[index];

        if (min > max || min == max)
        {
            throw std::runtime_error("Cannot visualize layer distance. Data is not valid");
        }
    }
};
}

std::string LayerDistance::getName() const
{
    return "Layer distance";
}

brayns::JsonSchema LayerDistance::getParamsSchema() const
{
    return _paramsSchema;
}

bool LayerDistance::isValidAtlas(const Atlas &atlas) const
{
    return atlas.getVoxelType() == VoxelType::LayerDistance;
}

std::shared_ptr<brayns::Model> LayerDistance::run(const Atlas &atlas, const brayns::JsonValue &payload) const
{
    auto &layerAtlas = static_cast<const LayerDistanceAtlas &>(atlas);
    auto params = ParamsParser::parse<LayerDistanceParameters>(payload);
    return LayerDistanceModelFactory::create(layerAtlas, params.type);
}
