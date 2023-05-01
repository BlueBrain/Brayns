/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/engine/colormethods/SolidColorMethod.h>
#include <brayns/engine/components/ColorSolid.h>
#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GenericColorSystem.h>
#include <brayns/engine/systems/GeometryDataSystem.h>

#include <api/coloring/handlers/ComposedColorHandler.h>
#include <api/coloring/methods/BrainDatasetColorMethod.h>
#include <api/coloring/methods/IdColorMethod.h>
#include <components/BrainColorData.h>
#include <components/CircuitIds.h>
#include <components/ColorHandler.h>

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
