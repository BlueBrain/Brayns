/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "SampleNeuronInstance.h"

#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>

#include <plugin/api/MaterialUtils.h>
#include <plugin/io/morphology/neuron/NeuronMaterialMap.h>

SampleNeuronInstance::SampleNeuronInstance(
    std::vector<brayns::Sphere>&& geometry,
    const std::shared_ptr<SampleSharedData>& data)
    : _samples(std::move(geometry))
    , _data(data)
{
}

void SampleNeuronInstance::mapSimulation(
    const size_t globalOffset, const std::vector<uint16_t>& sectionOffsets,
    const std::vector<uint16_t>& sectionCompartments)
{
    for (auto& geomSection : _data->sectionMap)
    {
        const auto& segments = geomSection.second;
        // No section level information (soma report, spike simulation, etc.)
        // or dealing with soma
        if (geomSection.first <= -1 || sectionOffsets.empty() ||
            static_cast<size_t>(geomSection.first) > sectionOffsets.size() - 1)
        {
            for (const auto& segment : segments)
                _samples[segment].userData = globalOffset;
        }
        else
        {
            const double step =
                static_cast<double>(sectionCompartments[geomSection.first]) /
                static_cast<double>(segments.size());

            const size_t sectionOffset = sectionOffsets[geomSection.first];
            for (size_t i = 0; i < segments.size(); ++i)
            {
                const auto compartment =
                    static_cast<size_t>(std::floor(step * i));
                const auto finalOffset =
                    globalOffset + (sectionOffset + compartment);
                _samples[segments[i]].userData = finalOffset;
            }
        }
    }
}

ElementMaterialMap::Ptr SampleNeuronInstance::addToModel(
    brayns::Model& model) const
{
    // Add geometries to the model
    std::unordered_map<NeuronSection, size_t> sectionToMat;
    for (const auto& entry : _data->sectionTypeMap)
    {
        const auto materialId = CircuitExplorerMaterial::create(model);
        sectionToMat[entry.first] = materialId;
        for (const auto geomIdx : entry.second)
            model.addSphere(materialId, _samples[geomIdx]);
    }

    const auto updateMaterialMap = [&](const NeuronSection section,
                                       size_t& buffer) {
        auto it = sectionToMat.find(section);
        if (it != sectionToMat.end())
            buffer = it->second;
    };
    auto materialMap = std::make_unique<NeuronMaterialMap>();
    updateMaterialMap(NeuronSection::SOMA, materialMap->soma);
    updateMaterialMap(NeuronSection::AXON, materialMap->axon);
    updateMaterialMap(NeuronSection::DENDRITE, materialMap->dendrite);
    updateMaterialMap(NeuronSection::APICAL_DENDRITE,
                      materialMap->apicalDendrite);
    return materialMap;
}

size_t SampleNeuronInstance::getSectionSegmentCount(const int32_t section) const
{
    auto it = _data->sectionMap.find(section);
    if (it == _data->sectionMap.end())
        throw std::invalid_argument("Section " + std::to_string(section) +
                                    "not found");

    return std::max(it->second.size(), 1ul) - 1;
}

MorphologyInstance::SegmentPoints SampleNeuronInstance::getSegment(
    const int32_t section, const uint32_t segment) const
{
    auto it = _data->sectionMap.find(section);
    if (it == _data->sectionMap.end())
        throw std::invalid_argument("Section " + std::to_string(section) +
                                    " not found");

    if (std::max(it->second.size(), 1ul) - 1 <= segment)
        throw std::invalid_argument("Section " + std::to_string(section) +
                                    " "
                                    "Segment " +
                                    std::to_string(segment) + " not found");

    const auto& start = _samples[it->second[segment]];
    const auto& end = _samples[it->second[segment + 1]];
    return std::make_pair(&start.center, &end.center);
}

uint64_t SampleNeuronInstance::getSegmentSimulationOffset(
    const int32_t section, const uint32_t segment) const
{
    auto it = _data->sectionMap.find(section);
    if (it == _data->sectionMap.end())
        throw std::invalid_argument("Section " + std::to_string(section) +
                                    " not found");

    if (it->second.size() <= segment)
        throw std::invalid_argument("Section " + std::to_string(section) +
                                    " "
                                    "Segment " +
                                    std::to_string(segment) + " not found");

    return _samples[it->second[segment]].userData;
}
