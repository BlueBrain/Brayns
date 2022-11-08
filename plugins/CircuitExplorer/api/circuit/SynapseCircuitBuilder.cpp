/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/GeometryCommitSystem.h>
#include <brayns/engine/systems/GeometryInitSystem.h>

#include <api/coloring/handlers/ComposedColorHandler.h>
#include <components/BrainColorData.h>
#include <components/CircuitIds.h>
#include <components/ColorHandler.h>

namespace
{
class ModelBuilder
{
public:
    ModelBuilder(brayns::Model &model)
        : _model(model)
    {
    }

    void addGeometry(std::map<uint64_t, std::vector<brayns::Sphere>> groupedSynapses)
    {
        auto &components = _model.getComponents();

        auto &ids = components.add<CircuitIds>();
        ids.elements.reserve(groupedSynapses.size());

        auto &geometries = components.add<brayns::Geometries>();
        geometries.elements.reserve(groupedSynapses.size());

        for (auto &[id, primitives] : groupedSynapses)
        {
            ids.elements.push_back(id);
            geometries.elements.push_back(std::move(primitives));
        }
    }

    void addColoring(std::unique_ptr<IBrainColorData> colorData)
    {
        auto &components = _model.getComponents();
        components.add<ColorHandler>(std::make_unique<ComposedColorHandler>());
        components.add<BrainColorData>(std::move(colorData));
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

void SynapseCircuitBuilder::build(
    brayns::Model &model,
    std::map<uint64_t, std::vector<brayns::Sphere>> groupedSynapses,
    std::unique_ptr<IBrainColorData> colorData)
{
    auto builder = ModelBuilder(model);
    builder.addGeometry(std::move(groupedSynapses));
    builder.addColoring(std::move(colorData));
    builder.addSystems();
}
