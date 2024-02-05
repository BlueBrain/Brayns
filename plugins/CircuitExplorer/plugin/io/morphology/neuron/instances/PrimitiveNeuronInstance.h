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

#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/Sphere.h>

#include <unordered_map>
#include <unordered_set>

enum class PrimitiveType : uint8_t
{
    SPHERE = 0,
    CYLINDER = 1,
    CONE = 2
};

struct PrimitiveGeometry
{
    PrimitiveType type;
    size_t index;
};

struct PrimitiveSharedData
{
    std::vector<PrimitiveGeometry> geometries;
    std::unordered_map<NeuronSection, std::vector<size_t>> sectionTypeMap;
    std::unordered_map<int32_t, std::vector<size_t>> sectionMap;
};

/**
 * @brief The PrimitiveNeuronInstance class represents a cell 3D shape based
 *        on spheres, cones and cylinders
 */
class PrimitiveNeuronInstance : public MorphologyInstance
{
public:
    PrimitiveNeuronInstance(std::vector<brayns::Sphere>&& spheres,
                            std::vector<brayns::Cylinder>&& cylinders,
                            std::vector<brayns::Cone>&& cones,
                            const std::shared_ptr<PrimitiveSharedData>& data);

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
    const brayns::Vector3f& _getGeometryP0(const PrimitiveGeometry& g) const
        noexcept;
    const brayns::Vector3f& _getGeometryP1(const PrimitiveGeometry& g) const
        noexcept;

    void _setSimulationOffset(const PrimitiveGeometry& geom,
                              const uint64_t offset) noexcept;

    std::vector<brayns::Sphere> _spheres;
    std::vector<brayns::Cylinder> _cylinders;
    std::vector<brayns::Cone> _cones;
    std::shared_ptr<PrimitiveSharedData> _data;
};
