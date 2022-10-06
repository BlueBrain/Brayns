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

#include "FlatmapOverlap.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/geometry/types/Box.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryCommitSystem.h>
#include <brayns/engine/systems/GeometryInitSystem.h>

#include <api/atlases/FlatmapAtlas.h>

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

class OverlappingAreas
{
public:
    using OverlapMap = std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::vector<size_t>>>;
    static OverlapMap compute(const FlatmapAtlas &atlas)
    {
        auto map = OverlapMap();

        auto validVoxelsIndices = ValidVoxels::getIndexList(atlas);
        for (auto index : validVoxelsIndices)
        {
            auto &coordinate = atlas[index];
            auto &overlapList = map[coordinate.x][coordinate.y];
            overlapList.push_back(index);
        }

        return map;
    }
};

class PrimitiveBuilder
{
public:
    static std::vector<brayns::Box> fromOverlapMap(const FlatmapAtlas &atlas, const OverlappingAreas::OverlapMap &map)
    {
        auto geometry = std::vector<brayns::Box>();
        geometry.reserve(_countTotalGeometries(map));

        for (auto &[x, subMap] : map)
        {
            for (auto &[y, overlapIndices] : subMap)
            {
                auto localVoxels = _generatePrimitives(atlas, overlapIndices);
                geometry.insert(geometry.end(), localVoxels.begin(), localVoxels.end());
            }
        }
        return geometry;
    }

private:
    static size_t _countTotalGeometries(const OverlappingAreas::OverlapMap &map)
    {
        size_t result = 0;

        for (auto &[x, subMap] : map)
        {
            for (auto &[y, overlapIndices] : subMap)
            {
                result += overlapIndices.size();
            }
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

class ModelBuilder
{
public:
    static std::unique_ptr<brayns::Model> build(std::vector<brayns::Box> primitives)
    {
        auto model = std::make_unique<brayns::Model>();

        auto &components = model->getComponents();

        auto &geometries = components.add<brayns::Geometries>();
        auto &geometry = geometries.elements.emplace_back(std::move(primitives));
        auto &views = components.add<brayns::GeometryViews>();
        auto &view = views.elements.emplace_back(geometry);
        view.setColor(brayns::Vector4f(1.f, 0.f, 0.f, 1.f));

        auto &systems = model->getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        systems.setInitSystem<brayns::GeometryInitSystem>();
        systems.setCommitSystem<brayns::GeometryCommitSystem>();

        return model;
    }
};
}

std::string FlatmapOverlap::getName() const
{
    return "Flatmap overlapping areas";
}

bool FlatmapOverlap::isAtlasValid(const Atlas &atlas) const
{
    return atlas.getVoxelType() == VoxelType::flatmap;
}

std::unique_ptr<brayns::Model> FlatmapOverlap::run(const Atlas &atlas, const brayns::JsonValue &payload) const
{
    (void)payload;
    assert(dynamic_cast<const FlatmapAtlas *>(&atlas));

    auto &flatmapAtlas = static_cast<const FlatmapAtlas &>(atlas);
    auto overlapAreas = OverlappingAreas::compute(flatmapAtlas);
    auto primitives = PrimitiveBuilder::fromOverlapMap(flatmapAtlas, overlapAreas);

    return ModelBuilder::build(std::move(primitives));
}
