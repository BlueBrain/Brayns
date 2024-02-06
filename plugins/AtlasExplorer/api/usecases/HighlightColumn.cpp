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

#include "HighlightColumn.h"

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/geometry/types/Box.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryDataSystem.h>

#include <api/ModelType.h>
#include "common/ParamsParser.h"

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
class CoordinatesValidator
{
public:
    static void validate(const HighlightColumParams &params, const brayns::Vector3ui &size)
    {
        auto &mainCoords = params.xz_coordinate;
        auto mainX = static_cast<uint32_t>(mainCoords.x);
        auto mainZ = static_cast<uint32_t>(mainCoords.y);

        if (size.x <= mainX || size.z <= mainZ)
        {
            throw std::invalid_argument("Column coordinates are out of volume bounds");
        }

        auto &neighbours = params.neighbours;
        for (auto &neighbour : neighbours)
        {
            auto neighbourCoords = neighbour.relative_xz;
            auto nx = mainX + neighbourCoords.x;
            auto nz = mainZ + neighbourCoords.y;
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
    ColumnBoundFinder(const brayns::Vector3ui &size, const brayns::Vector3f &spacing):
        _size(size),
        _spacing(spacing)
    {
    }

    brayns::Box find(const brayns::Vector2ui &xz) const noexcept
    {
        auto x = static_cast<float>(xz.x);
        auto z = static_cast<float>(xz.y);
        auto y = static_cast<float>(_size.y);

        brayns::Vector3f min(x, -1.f, z);
        brayns::Vector3f max(x + 1.f, y, z + 1.f);

        return brayns::Box{min * _spacing, max * _spacing};
    }

private:
    brayns::Vector3ui _size;
    brayns::Vector3f _spacing;
};

class ModelBuilder
{
public:
    explicit ModelBuilder(brayns::Model &model):
        _model(model)
    {
    }

    void addComponents(std::vector<brayns::Box> primitives, const std::vector<brayns::Vector4f> &colors)
    {
        auto &components = _model.getComponents();
        auto &geometries = components.add<brayns::Geometries>();
        auto &geometry = geometries.elements.emplace_back(std::move(primitives));
        auto &views = components.add<brayns::GeometryViews>();
        auto &view = views.elements.emplace_back(geometry);
        view.setColorPerPrimitive(ospray::cpp::CopiedData(colors));
    }

    void addSystems()
    {
        auto &systems = _model.getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        systems.setDataSystem<brayns::GeometryDataSystem>();
    }

private:
    brayns::Model &_model;
};
}

std::string HighlightColumn::getName() const
{
    return "Highlight column";
}

brayns::JsonSchema HighlightColumn::getParamsSchema() const
{
    return _paramsSchema;
}

bool HighlightColumn::isValidAtlas(const Atlas &atlas) const
{
    (void)atlas;
    return true;
}

std::shared_ptr<brayns::Model> HighlightColumn::run(const Atlas &atlas, const brayns::JsonValue &payload) const
{
    const auto params = ParamsParser::parse<HighlightColumParams>(payload);
    CoordinatesValidator::validate(params, atlas.getSize());

    const auto numColumns = params.neighbours.size() + 1;
    std::vector<brayns::Box> geometry;
    geometry.reserve(numColumns);
    std::vector<brayns::Vector4f> colors;
    colors.reserve(numColumns);

    ColumnBoundFinder finder(atlas.getSize(), atlas.getSpacing());
    auto coords = params.xz_coordinate;

    geometry.push_back(finder.find(coords));
    colors.push_back(params.color);

    for (const auto &neighbour : params.neighbours)
    {
        const auto nCoords = coords + neighbour.relative_xz;
        geometry.push_back(finder.find(nCoords));
        colors.push_back(neighbour.color);
    }

    auto model = std::make_shared<brayns::Model>(ModelType::atlas);

    ModelBuilder builder(*model);
    builder.addComponents(std::move(geometry), colors);
    builder.addSystems();

    return model;
}
