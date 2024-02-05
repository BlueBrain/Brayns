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

#pragma once

#include <plugin/io/morphology/MorphologyInstance.h>
#include <plugin/io/morphology/neuron/NeuronSection.h>

#include <brayns/common/geometry/SDFGeometry.h>

#include <unordered_map>

/**
 * @brief The SDFMorphologyInstance class represents a cell 3D shape based
 *        on SDF Geometry
 */
struct SDFSharedData
{
    std::vector<std::vector<size_t>> neighbours;
    std::unordered_map<NeuronSection, std::vector<size_t>> sectionTypeMap;
    std::unordered_map<int32_t, std::vector<size_t>> sectionGeometries;
};

class SDFNeuronInstance : public MorphologyInstance
{
public:
    SDFNeuronInstance(std::vector<brayns::SDFGeometry>&& sdfGeometries,
                      const std::shared_ptr<SDFSharedData>& sdfData);

    void mapSimulation(const size_t globalOffset,
                       const std::vector<uint16_t>& sectionOffsets,
                       const std::vector<uint16_t>& sectionCompartments) final;

    ElementMaterialMap::Ptr addToModel(brayns::Model& model) const final;

    size_t getSectionSegmentCount(const int32_t section) const final;

    MorphologyInstance::SegmentPoints getSegment(
        const int32_t section, const uint32_t segment) const final;

    uint64_t getSegmentSimulationOffset(const int32_t section,
                                        const uint32_t segment) const final;

private:
    std::vector<brayns::SDFGeometry> _sdfGeometries;
    std::shared_ptr<SDFSharedData> _sdfData;
};
