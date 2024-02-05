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

#include "PrimitiveNeuronInstance.h"

#include <brayns/engine/Model.h>

#include <plugin/api/MaterialUtils.h>
#include <plugin/io/morphology/neuron/NeuronMaterialMap.h>

PrimitiveNeuronInstance::PrimitiveNeuronInstance(
    std::vector<brayns::Sphere>&& spheres,
    std::vector<brayns::Cylinder>&& cylinders,
    std::vector<brayns::Cone>&& cones,
    const std::shared_ptr<PrimitiveSharedData>& data)
    : _spheres(std::move(spheres))
    , _cylinders(std::move(cylinders))
    , _cones(std::move(cones))
    , _data(data)
{
}

void PrimitiveNeuronInstance::mapSimulation(
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
                _setSimulationOffset(_data->geometries[segment], globalOffset);
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
                _setSimulationOffset(_data->geometries[segments[i]],
                                     finalOffset);
            }
        }
    }
}

ElementMaterialMap::Ptr PrimitiveNeuronInstance::addToModel(
    brayns::Model& model) const
{
    std::unordered_map<NeuronSection, size_t> sectionToMat;
    for (const auto& sectionTypeGeom : _data->sectionTypeMap)
    {
        const auto materialId = CircuitExplorerMaterial::create(model);
        sectionToMat[sectionTypeGeom.first] = materialId;
        for (const auto geometryIdx : sectionTypeGeom.second)
        {
            const auto& primitive = _data->geometries[geometryIdx];
            switch (primitive.type)
            {
            case PrimitiveType::SPHERE:
                model.addSphere(materialId, _spheres[primitive.index]);
                break;
            case PrimitiveType::CONE:
                model.addCone(materialId, _cones[primitive.index]);
                break;
            case PrimitiveType::CYLINDER:
                model.addCylinder(materialId, _cylinders[primitive.index]);
                break;
            }
        }
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

size_t PrimitiveNeuronInstance::getSectionSegmentCount(
    const int32_t section) const
{
    auto it = _data->sectionMap.find(section);
    if (it == _data->sectionMap.end())
        throw std::invalid_argument("Section " + std::to_string(section) +
                                    "not found");

    return it->second.size();
}

MorphologyInstance::SegmentPoints PrimitiveNeuronInstance::getSegment(
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

    const auto& geom = _data->geometries[it->second[segment]];
    const auto& start = _getGeometryP0(geom);
    const auto& end = _getGeometryP1(geom);
    return std::make_pair(&start, &end);
}

uint64_t PrimitiveNeuronInstance::getSegmentSimulationOffset(
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

    const auto& geom = _data->geometries[it->second[segment]];
    switch (geom.type)
    {
    case PrimitiveType::SPHERE:
        return _spheres[geom.index].userData;
    case PrimitiveType::CYLINDER:
        return _cylinders[geom.index].userData;
    default:
        return _cones[geom.index].userData;
    }
}

const brayns::Vector3f& PrimitiveNeuronInstance::_getGeometryP0(
    const PrimitiveGeometry& g) const noexcept
{
    switch (g.type)
    {
    case PrimitiveType::SPHERE:
        return _spheres[g.index].center;
    case PrimitiveType::CYLINDER:
        return _cylinders[g.index].center;
    default:
        return _cones[g.index].center;
    }
}

//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wterminate"
const brayns::Vector3f& PrimitiveNeuronInstance::_getGeometryP1(
    const PrimitiveGeometry& g) const noexcept
{
    switch (g.type)
    {
    case PrimitiveType::SPHERE:
        return _spheres[g.index].center;
    case PrimitiveType::CYLINDER:
        return _cylinders[g.index].up;
    default:
        return _cones[g.index].up;
    }
}
//#pragma GCC diagnostic pop

void PrimitiveNeuronInstance::_setSimulationOffset(
    const PrimitiveGeometry& geom, const uint64_t offset) noexcept
{
    switch (geom.type)
    {
    case PrimitiveType::SPHERE:
        _spheres[geom.index].userData = offset;
        break;
    case PrimitiveType::CYLINDER:
        _cylinders[geom.index].userData = offset;
        break;
    case PrimitiveType::CONE:
        _cones[geom.index].userData = offset;
        break;
    }
}
