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

#include <brayns/engine/geometries/Primitive.h>

#include <plugin/io/morphology/neuron/NeuronMorphology.h>
#include <plugin/io/morphology/neuron/components/MorphologyCircuitComponent.h>

struct PrimitiveNeuronGeometry
{
    std::vector<brayns::Primitive> geometry;
    std::vector<MorphologySectionMapping> sectionMapping;
    std::unordered_map<int32_t, std::vector<size_t>> sectionSegmentMapping;
};

/**
 * @brief The PrimitiveGeometryBuilder class is a builder that transform a Morphology object into primitive geometry
 */
class PrimitiveNeuronBuilder
{
public:
    /**
     * @brief PrimitiveNeuronBuilder
     * @param morphology
     */
    PrimitiveNeuronBuilder(const NeuronMorphology &morphology);

    /**
     * @brief instantiate
     * @param position
     * @param rotation
     * @return
     */
    PrimitiveNeuronGeometry instantiate(
            const brayns::Vector3f &position, const brayns::Quaternion &rotation) const;

private:
    PrimitiveNeuronGeometry _data;
};
