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

#include "SomaCircuitBuilder.h"

#include <brayns/core/engine/colormethods/SolidColorMethod.h>
#include <brayns/core/engine/components/ColorSolid.h>
#include <brayns/core/engine/components/Geometries.h>
#include <brayns/core/engine/geometry/types/Sphere.h>
#include <brayns/core/engine/systems/GenericBoundsSystem.h>
#include <brayns/core/engine/systems/GenericColorSystem.h>
#include <brayns/core/engine/systems/GeometryDataSystem.h>

#include <brayns/circuits/api/ModelType.h>
#include <brayns/circuits/api/coloring/handlers/SimpleColorHandler.h>
#include <brayns/circuits/api/coloring/methods/BrainDatasetColorMethod.h>
#include <brayns/circuits/api/coloring/methods/IdColorMethod.h>
#include <brayns/circuits/components/BrainColorData.h>
#include <brayns/circuits/components/CircuitIds.h>
#include <brayns/circuits/components/ColorHandler.h>
#include <brayns/circuits/systems/NeuronInspectSystem.h>

namespace
{
class ModelBuilder
{
public:
    explicit ModelBuilder(brayns::Model &model):
        _components(model.getComponents()),
        _systems(model.getSystems()),
        _modelType(model.getType())
    {
    }

    void addIds(std::vector<uint64_t> ids)
    {
        _components.add<CircuitIds>(std::move(ids));
    }

    void addGeometry(std::vector<brayns::Sphere> primitives)
    {
        _components.add<brayns::Geometries>(std::move(primitives));
        _systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        _systems.setDataSystem<brayns::GeometryDataSystem>();
        _systems.setInspectSystem<SomaInspectSystem>();
    }

    void addColoring(std::unique_ptr<IBrainColorData> data)
    {
        auto availableMethods = data->getMethods();
        auto colorMethods = brayns::ColorMethodList();
        colorMethods.reserve(availableMethods.size() + 2);

        colorMethods.push_back(std::make_unique<brayns::SolidColorMethod>());
        colorMethods.push_back(std::make_unique<IdColorMethod>());
        for (auto method : availableMethods)
        {
            colorMethods.push_back(std::make_unique<BrainDatasetColorMethod>(method));
        }

        _systems.setColorSystem<brayns::GenericColorSystem>(std::move(colorMethods));

        _components.add<ColorHandler>(std::make_unique<SimpleColorHandler>());
        _components.add<BrainColorData>(std::move(data));
    }

    void addDefaultColor()
    {
        if (_modelType == ModelType::neurons)
        {
            _components.add<brayns::ColorSolid>(brayns::Vector4f(1.f, 1.f, 0.f, 1.f));
            return;
        }

        _components.add<brayns::ColorSolid>(brayns::Vector4f(0.55f, 0.7f, 1.f, 1.f));
    }

private:
    brayns::Components &_components;
    brayns::Systems &_systems;
    const std::string &_modelType;
};
}

std::vector<CellCompartments> SomaCircuitBuilder::build(brayns::Model &model, Context context)
{
    auto &positions = context.positions;
    auto radius = context.radius;

    auto compartments = std::vector<CellCompartments>();
    compartments.reserve(positions.size());
    auto geometry = std::vector<brayns::Sphere>();
    geometry.reserve(positions.size());

    for (auto &position : positions)
    {
        geometry.push_back({position, radius});
        compartments.push_back({1, {{-1, 0, 1}}});
    }

    auto builder = ModelBuilder(model);
    builder.addIds(std::move(context.ids));
    builder.addGeometry(std::move(geometry));
    builder.addColoring(std::move(context.colorData));
    builder.addDefaultColor();

    return compartments;
}
