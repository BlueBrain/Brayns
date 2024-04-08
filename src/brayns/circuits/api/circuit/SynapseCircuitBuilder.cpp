/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include "SynapseCircuitBuilder.h"

#include <brayns/core/engine/colormethods/SolidColorMethod.h>
#include <brayns/core/engine/components/ColorSolid.h>
#include <brayns/core/engine/components/Geometries.h>
#include <brayns/core/engine/systems/GenericBoundsSystem.h>
#include <brayns/core/engine/systems/GenericColorSystem.h>
#include <brayns/core/engine/systems/GeometryDataSystem.h>

#include <brayns/circuits/api/coloring/handlers/ComposedColorHandler.h>
#include <brayns/circuits/api/coloring/methods/BrainDatasetColorMethod.h>
#include <brayns/circuits/api/coloring/methods/IdColorMethod.h>
#include <brayns/circuits/components/BrainColorData.h>
#include <brayns/circuits/components/CircuitIds.h>
#include <brayns/circuits/components/ColorHandler.h>

namespace
{
class ModelBuilder
{
public:
    explicit ModelBuilder(brayns::Model &model):
        _components(model.getComponents()),
        _systems(model.getSystems())
    {
    }

    void addGeometry(std::map<uint64_t, std::vector<brayns::Sphere>> groupedSynapses)
    {
        auto &ids = _components.add<CircuitIds>();
        ids.elements.reserve(groupedSynapses.size());

        auto &geometries = _components.add<brayns::Geometries>();
        geometries.elements.reserve(groupedSynapses.size());

        for (auto &[id, primitives] : groupedSynapses)
        {
            ids.elements.push_back(id);
            geometries.elements.emplace_back(std::move(primitives));
        }

        _systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Geometries>>();
        _systems.setDataSystem<brayns::GeometryDataSystem>();
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

        _components.add<ColorHandler>(std::make_unique<ComposedColorHandler>());
        _components.add<BrainColorData>(std::move(data));
        _components.add<brayns::ColorSolid>(brayns::Vector4f(1.f));
    }

private:
    brayns::Components &_components;
    brayns::Systems &_systems;
};
}

void SynapseCircuitBuilder::build(brayns::Model &model, Context context)
{
    auto builder = ModelBuilder(model);
    builder.addGeometry(std::move(context.groupedSynapses));
    builder.addColoring(std::move(context.colorData));
}
