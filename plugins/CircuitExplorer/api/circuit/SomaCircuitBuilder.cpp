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
#include "colorhandlers/SomaColorHandler.h"

#include <components/CircuitColorComponent.h>
#include <components/SomaCircuitComponent.h>

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
    SomaCircuitBuilder::load(const Context &context, brayns::Model &model, std::unique_ptr<IColorData> colorData)
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

    auto &somaCircuit = model.addComponent<SomaCircuitComponent>();
    somaCircuit.setSomas(ids, std::move(geometry));

    auto colorHandler = std::make_unique<SomaColorHandler>(somaCircuit);
    model.addComponent<CircuitColorComponent>(std::move(colorData), std::move(colorHandler));

    return result;
}
