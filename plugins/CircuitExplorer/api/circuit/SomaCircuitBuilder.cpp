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

#include "SomaCircuitBuilder.h"

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/geometry/types/Sphere.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryCommitSystem.h>
#include <brayns/engine/systems/GeometryInitSystem.h>

#include <api/coloring/handlers/SimpleColorHandler.h>
#include <components/BrainColorData.h>
#include <components/CircuitIds.h>
#include <components/ColorHandler.h>
#include <systems/NeuronInspectSystem.h>

namespace
{
class ModelBuilder
{
public:
    ModelBuilder(brayns::Model &model)
        : _model(model)
    {
    }

    void addIds(const std::vector<uint64_t> &ids)
    {
        _model.getComponents().add<CircuitIds>(ids);
    }

    void addGeometry(std::vector<brayns::Sphere> primitives)
    {
        auto &components = _model.getComponents();
        auto &geometries = components.add<brayns::Geometries>();
        geometries.elements.emplace_back(std::move(primitives));
    }

    void addColoring(std::unique_ptr<IBrainColorData> data)
    {
        auto &components = _model.getComponents();
        components.add<ColorHandler>(std::make_unique<SimpleColorHandler>());
        components.add<BrainColorData>(std::move(data));
    }

    void addSystems()
    {
        auto &systems = _model.getSystems();
        systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        systems.setInitSystem<brayns::GeometryInitSystem>();
        systems.setCommitSystem<brayns::GeometryCommitSystem>();
        systems.setInspectSystem<SomaInspectSystem>();
    }

private:
    brayns::Model &_model;
};
}

SomaCircuitBuilder::Context::Context(
    const std::vector<uint64_t> &ids,
    const std::vector<brayns::Vector3f> &positions,
    float radius)
    : ids(ids)
    , positions(positions)
    , radius(radius)
{
}

std::vector<CellCompartments>
    SomaCircuitBuilder::load(const Context &context, brayns::Model &model, std::unique_ptr<IBrainColorData> colorData)
{
    const auto &ids = context.ids;
    const auto &positions = context.positions;
    const auto radius = context.radius;

    std::vector<CellCompartments> result(ids.size());
    std::vector<brayns::Sphere> geometry(ids.size());

#pragma omp parallel for
    for (size_t i = 0; i < ids.size(); ++i)
    {
        const auto &pos = positions[i];
        auto &somaSphere = geometry[i];
        somaSphere.center = pos;
        somaSphere.radius = radius;

        auto &compartment = result[i];

        compartment.numItems = 1;
        compartment.sectionSegments[-1].push_back(0);
    }

    auto builder = ModelBuilder(model);
    builder.addIds(ids);
    builder.addGeometry(std::move(geometry));
    builder.addColoring(std::move(colorData));
    builder.addSystems();

    return result;
}
