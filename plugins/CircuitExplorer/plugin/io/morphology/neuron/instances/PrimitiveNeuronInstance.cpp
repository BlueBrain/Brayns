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

#include "PrimitiveNeuronInstance.h"

#include <brayns/engine/Model.h>

#include <plugin/io/morphology/neuron/components/MorphologyCircuitComponent.h>

PrimitiveNeuronInstance::PrimitiveNeuronInstance(std::vector<brayns::Primitive> geometry, NeuronGeometryMapping data)
    : _geometry(std::move(geometry))
    , _data(std::move(data))
{
}

std::vector<uint64_t> PrimitiveNeuronInstance::mapSimulation(const SimulationMapping &mapping) const
{
    const auto offset = mapping.globalOffset;
    const auto &localOffsets = mapping.offsets;
    const auto &compartments = mapping.compartments;

    std::vector<uint64_t> result (_geometry.size(), offset);

    for (const auto& [sectionId, segments] : _data.sectionSegmentMapping)
    {
        // No section level information (soma report, spike simulation, etc.) or dealing with soma
        if (sectionId <= -1 || localOffsets.empty() || static_cast<size_t>(sectionId) > localOffsets.size() - 1)
        {
            continue;
        }
        else
        {
            const auto numSegments = segments.size();
            const auto numCompartments = compartments[sectionId];
            const auto step = static_cast<double>(numCompartments) / static_cast<double>(numSegments);
            const size_t sectionOffset = localOffsets[sectionId];
            for (size_t i = 0; i < segments.size(); ++i)
            {
                const auto compartment = static_cast<size_t>(std::floor(step * i));
                const auto finalOffset = offset + (sectionOffset + compartment);
                const auto segmentIndex = segments[i];
                result[segmentIndex] = finalOffset;
            }
        }
    }

    return result;
}

/*
void PrimitiveNeuronInstance::addToModel(uint64_t id, brayns::Model &model)
{
    auto &renderComponent = model.getComponent<MorphologyCircuitComponent>();
    renderComponent.addMorphology(id, _geometry, _data.sectionMapping);
}
*/
