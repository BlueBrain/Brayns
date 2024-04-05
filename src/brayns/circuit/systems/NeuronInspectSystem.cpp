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

#include "NeuronInspectSystem.h"

#include <brayns/core/engine/components/GeometryViews.h>

#include <brayns/circuit/components/CircuitIds.h>

#include <algorithm>

namespace
{
struct ResultParameters
{
    static inline const std::string neuronId = "neuron_id";
};

class HittedMorphologyFinder
{
public:
    static size_t findIndex(const brayns::InspectContext &context, brayns::Components &components)
    {
        auto &views = components.get<brayns::GeometryViews>();
        auto &elements = views.elements;
        auto &hitView = context.model;

        auto it = std::find_if(
            elements.begin(),
            elements.end(),
            [&](const brayns::GeometryView &view)
            {
                auto &handle = view.getHandle();
                return handle.handle() == hitView.handle();
            });

        assert(it != elements.end());
        return std::distance(elements.begin(), it);
    }
};
}

brayns::InspectResultData SomaInspectSystem::execute(
    const brayns::InspectContext &context,
    brayns::Components &components)
{
    auto &ids = components.get<CircuitIds>();
    brayns::InspectResultData result;
    result.set(ResultParameters::neuronId, ids.elements[context.primitiveIndex]);
    return result;
}

brayns::InspectResultData MorphologyInspectSystem::execute(
    const brayns::InspectContext &context,
    brayns::Components &components)
{
    auto hittedIndex = HittedMorphologyFinder::findIndex(context, components);
    auto &ids = components.get<CircuitIds>();
    brayns::InspectResultData result;
    result.set(ResultParameters::neuronId, ids.elements[hittedIndex]);
    return result;
}
