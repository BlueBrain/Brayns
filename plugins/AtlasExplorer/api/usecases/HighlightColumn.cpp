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

#include "HighlightColumn.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/GeometryViews.h>
#include <brayns/engine/geometry/types/Box.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryCommitSystem.h>
#include <brayns/engine/systems/GeometryInitSystem.h>

#include "common/ParamsParser.h"

namespace
{
class CoordinatesValidator
{
public:
    static void validate(const HighlighColumParams &params, const AtlasData &volume)
    {
        auto &size = volume.size;
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
    ColumnBoundFinder(const AtlasData &volume)
        : _size(volume.size)
        , _spacing(volume.spacing)
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
    ModelBuilder(brayns::Model &model)
        : _model(model)
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
        systems.setInitSystem<brayns::GeometryInitSystem>();
        systems.setCommitSystem<brayns::GeometryCommitSystem>();
    }

private:
    brayns::Model &_model;
};
}

std::string HighlightColumn::getName() const
{
    return "Highlight column";
}

bool HighlightColumn::isVolumeValid(const AtlasData &volume) const
{
    (void)volume;
    return true;
}

std::unique_ptr<brayns::Model> HighlightColumn::execute(const AtlasData &volume, const brayns::JsonValue &payload) const
{
    const auto params = ParamsParser::parse<HighlighColumParams>(payload);
    CoordinatesValidator::validate(params, volume);

    const auto numColumns = params.neighbours.size() + 1;
    std::vector<brayns::Box> geometry;
    geometry.reserve(numColumns);
    std::vector<brayns::Vector4f> colors;
    colors.reserve(numColumns);

    ColumnBoundFinder finder(volume);
    const auto coords = params.xz_coordinate;

    geometry.push_back(finder.find(coords));
    colors.push_back(params.color);

    for (const auto &neighbour : params.neighbours)
    {
        const auto nCoords = coords + neighbour.relative_xz;
        geometry.push_back(finder.find(nCoords));
        colors.push_back(neighbour.color);
    }

    auto model = std::make_unique<brayns::Model>();

    ModelBuilder builder(*model);
    builder.addComponents(std::move(geometry), colors);
    builder.addSystems();

    return model;
}
