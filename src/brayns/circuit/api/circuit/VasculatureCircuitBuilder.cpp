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

#include "VasculatureCircuitBuilder.h"

#include <brayns/core/engine/colormethods/SolidColorMethod.h>
#include <brayns/core/engine/components/ColorSolid.h>
#include <brayns/core/engine/components/Geometries.h>
#include <brayns/core/engine/geometry/types/Capsule.h>
#include <brayns/core/engine/model/Model.h>
#include <brayns/core/engine/systems/GenericBoundsSystem.h>
#include <brayns/core/engine/systems/GenericColorSystem.h>
#include <brayns/core/engine/systems/GeometryDataSystem.h>

#include <brayns/circuit/api/coloring/handlers/SimpleColorHandler.h>
#include <brayns/circuit/api/coloring/methods/IdColorMethod.h>
#include <brayns/circuit/api/coloring/methods/VasculatureSectionColorMethod.h>
#include <brayns/circuit/components/CircuitIds.h>
#include <brayns/circuit/components/ColorHandler.h>
#include <brayns/circuit/components/VasculatureSectionList.h>

namespace
{
class PrimitiveBuilder
{
public:
    static std::vector<brayns::Capsule> build(const VasculatureCircuitBuilder::Context &context)
    {
        auto size = context.startPos.size();
        auto primitives = std::vector<brayns::Capsule>();
        primitives.reserve(size);

        for (size_t i = 0; i < size; ++i)
        {
            auto &p0 = context.startPos[i];
            auto r0 = context.startRadii[i];
            auto &p1 = context.endPos[i];
            auto r1 = context.endRadii[i];
            primitives.push_back(brayns::CapsuleFactory::cone(p0, r0, p1, r1));
        }
        return primitives;
    }
};

class ModelBuilder
{
public:
    explicit ModelBuilder(brayns::Model &model):
        _components(model.getComponents()),
        _systems(model.getSystems())
    {
    }

    void addGeometry(std::vector<brayns::Capsule> primitives)
    {
        auto &geometries = _components.add<brayns::Geometries>();
        geometries.elements.emplace_back(std::move(primitives));
    }

    void addColoring()
    {
        _components.add<ColorHandler>(std::make_unique<SimpleColorHandler>());
        _components.add<brayns::ColorSolid>(brayns::Vector4f(1.f, 0.f, 0.f, 1.f));
    }

    void addSections(std::vector<VasculatureSection> sections)
    {
        _components.add<VasculatureSectionList>(std::move(sections));
    }

    void addIds(std::vector<uint64_t> ids)
    {
        _components.add<CircuitIds>(std::move(ids));
    }

    void addSystems()
    {
        auto colorMethods = brayns::ColorMethodList();
        colorMethods.push_back(std::make_unique<brayns::SolidColorMethod>());
        colorMethods.push_back(std::make_unique<IdColorMethod>());
        colorMethods.push_back(std::make_unique<VasculatureSectionColorMethod>());

        _systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        _systems.setDataSystem<brayns::GeometryDataSystem>();
        _systems.setColorSystem<brayns::GenericColorSystem>(std::move(colorMethods));
    }

private:
    brayns::Components &_components;
    brayns::Systems &_systems;
};
}

void VasculatureCircuitBuilder::build(brayns::Model &model, Context context)
{
    auto primitives = PrimitiveBuilder::build(context);

    auto builder = ModelBuilder(model);
    builder.addGeometry(std::move(primitives));
    builder.addIds(std::move(context.ids));
    builder.addColoring();
    builder.addSections(std::move(context.sections));
    builder.addSystems();
}
