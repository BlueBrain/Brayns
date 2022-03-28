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
#include <plugin/io/morphology/neuron/components/MorphologyCircuitComponent.h>

#include <brayns/engine/geometries/Primitive.h>

#include <unordered_map>

struct NeuronGeometryMapping
{
    std::vector<MorphologySectionMapping> sectionMapping;
    std::unordered_map<int32_t, std::vector<uint32_t>> sectionSegmentMapping;
};

/**
 * @brief The PrimitiveNeuronInstance class represents a cell 3D shape based
 *        on spheres, cones and cylinders
 */
class PrimitiveNeuronInstance final : public MorphologyInstance
{
public:
    PrimitiveNeuronInstance(std::vector<brayns::Primitive> prims, NeuronGeometryMapping data);

    void addToModel(uint64_t id, brayns::Model &model) override;

private:
    std::vector<brayns::Primitive> _geometry;
    NeuronGeometryMapping _data;
};
