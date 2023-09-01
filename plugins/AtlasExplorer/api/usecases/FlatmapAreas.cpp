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

#include "FlatmapAreas.h"

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/geometry/types/Box.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryDataSystem.h>

#include <api/ModelType.h>
#include <api/atlases/FlatmapAtlas.h>

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
class ValidVoxels
{
public:
    static std::vector<size_t> getIndexList(const FlatmapAtlas &atlas)
    {
        auto voxelCount = atlas.getVoxelCount();
        auto result = std::vector<size_t>();
        result.reserve(voxelCount);
        for (size_t i = 0; i < voxelCount; ++i)
        {
            if (!atlas.isValidVoxel(i))
            {
                continue;
            }
            result.push_back(i);
        }

        return result;
    }
};

using AreaMap = std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::vector<size_t>>>;

class AreaMapGenerator
{
public:
    static AreaMap fromAtlas(const FlatmapAtlas &atlas)
    {
        auto map = AreaMap();

        auto validVoxelsIndices = ValidVoxels::getIndexList(atlas);
        for (auto index : validVoxelsIndices)
        {
            auto &coordinate = atlas[index];
            auto &areaList = map[coordinate.x][coordinate.y];
            areaList.push_back(index);
        }

        return map;
    }
};

class PrimitiveBuilder
{
public:
    static std::vector<std::vector<brayns::Box>> build(const FlatmapAtlas &atlas)
    {
        auto map = AreaMapGenerator::fromAtlas(atlas);

        auto geometry = std::vector<std::vector<brayns::Box>>();
        geometry.reserve(_countTotalAreas(map));

        for (auto &[x, subMap] : map)
        {
            for (auto &[y, areaVoxelIndices] : subMap)
            {
                auto areaVoxels = _generatePrimitives(atlas, areaVoxelIndices);
                geometry.push_back(std::move(areaVoxels));
            }
        }
        return geometry;
    }

private:
    static size_t _countTotalAreas(const AreaMap &map)
    {
        size_t result = 0;

        for (auto &[x, subMap] : map)
        {
            result += subMap.size();
        }
        return result;
    }

    static std::vector<brayns::Box> _generatePrimitives(const FlatmapAtlas &atlas, const std::vector<size_t> &indices)
    {
        auto result = std::vector<brayns::Box>();
        result.reserve(indices.size());

        for (auto index : indices)
        {
            auto bounds = atlas.getVoxelBounds(index);
            result.push_back(brayns::Box{bounds.getMin(), bounds.getMax()});
        }
        return result;
    }
};

class ColorRoulette
{
public:
    static inline const std::vector<brayns::Vector4f> colors = {
        {1.f, 0.f, 0.f, 1.f},
        {0.f, 1.f, 0.f, 1.f},
        {0.f, 0.f, 1.f, 1.f},
        {1.f, 1.f, 0.f, 1.f},
        {1.f, 0.f, 1.f, 1.f},
        {0.f, 1.f, 1.f, 1.f}};

    const brayns::Vector4f &getNextColor() noexcept
    {
        auto retrive = _index;
        _index = (_index + 1) % colors.size();
        return colors[retrive];
    }

private:
    size_t _index = 0;
};

class ModelBuilder
{
public:
    static std::shared_ptr<brayns::Model> build(std::vector<std::vector<brayns::Box>> primitives)
    {
        auto model = std::make_shared<brayns::Model>(ModelType::atlas);

        auto &components = model->getComponents();

        auto &geometries = components.add<brayns::Geometries>();
        geometries.elements.reserve(primitives.size());
        auto &views = components.add<brayns::GeometryViews>();
        views.elements.reserve(primitives.size());
        auto roulette = ColorRoulette();
        for (auto &primitiveList : primitives)
        {
            auto &geometry = geometries.elements.emplace_back(std::move(primitiveList));
            auto &view = views.elements.emplace_back(geometry);
            view.setColor(roulette.getNextColor());
        }

        auto &systems = model->getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        systems.setDataSystem<brayns::GeometryDataSystem>();

        return model;
    }
};
}

std::string FlatmapAreas::getName() const
{
    return "Flatmap areas";
}

bool FlatmapAreas::isValidAtlas(const Atlas &atlas) const
{
    return atlas.getVoxelType() == VoxelType::Flatmap;
}

std::shared_ptr<brayns::Model> FlatmapAreas::run(const Atlas &atlas, const brayns::JsonValue &payload) const
{
    (void)payload;
    assert(dynamic_cast<const FlatmapAtlas *>(&atlas));

    auto &flatmapAtlas = static_cast<const FlatmapAtlas &>(atlas);
    auto primitives = PrimitiveBuilder::build(flatmapAtlas);

    return ModelBuilder::build(std::move(primitives));
}
