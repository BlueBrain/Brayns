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

#pragma once

#include <plugin/io/morphology/MorphologyInstance.h>
#include <plugin/io/morphology/neuron/NeuronGeometryMapping.h>

#include <brayns/engine/geometries/Primitive.h>

enum class PrimitiveType : uint8_t
{
    SPHERE = 0,
    CYLINDER = 1,
    CONE = 2
};

/**
 * @brief The PrimitiveNeuronInstance class represents a cell 3D shape based
 *        on spheres, cones and cylinders
 */
class PrimitiveNeuronInstance final : public MorphologyInstance
{
public:
    PrimitiveNeuronInstance(std::vector<brayns::Primitive> prims, NeuronGeometryMapping data);

    std::vector<uint64_t> mapSimulation(const SimulationMapping &mapping) const override;

    void addToModel(uint64_t id, brayns::Model &model) override;

    size_t getSectionSegmentCount(const int32_t section) const override;

    MorphologyInstance::SegmentPoints getSegment(const int32_t section, const uint32_t segment) const override;

private:
    std::vector<brayns::Primitive> _geometry;
    NeuronGeometryMapping _data;
};
