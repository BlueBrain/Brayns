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

#include "LayerDistance.h"

#include "common/ParamsParser.h"

#include <api/atlases/LayerDistanceAtlas.h>

namespace
{
class LayerDistanceModelFactory
{
public:
    static std::shared_ptr<brayns::Model> create(const LayerDistanceAtlas &atlas, const LayerDistanceParameters &param)
    {
        auto type = param.type;
        if (type == LayerDistanceType::lower)
        {
            return _create(atlas, 0);
        }

        return _create(atlas, 1);
    }

private:
    static std::shared_ptr<brayns::Model> _create(const LayerDistanceAtlas &atlas, size_t index)
    {
        auto size = atlas.getVoxelCount();

        for (size_t i = 0; i < size; ++i)
        {
            if (!atlas.isValidVoxel(i))
            {
                continue;
            }

            auto bounds = atlas.getVoxelBounds(i);
            auto value = atlas[i];
        }
    }
};
}

std::string LayerDistance::getName() const
{
    return "Layer distance";
}

bool LayerDistance::isValidAtlas(const Atlas &atlas) const
{
    return atlas.getVoxelType() == VoxelType::layerDistance;
}

std::shared_ptr<brayns::Model> LayerDistance::run(const Atlas &atlas, const brayns::JsonValue &payload) const
{
    auto &layerAtlas = static_cast<const LayerDistanceAtlas &>(atlas);
    auto params = ParamsParser::parse<LayerDistanceParameters>(payload);
    return LayerDistanceModelFactory::create(layerAtlas, params);
}
