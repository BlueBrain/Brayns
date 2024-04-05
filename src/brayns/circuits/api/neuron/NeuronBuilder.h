/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "NeuronGeometry.h"
#include "NeuronMorphology.h"

template<typename T>
constexpr bool NeuronBuilderNotSpecialized = false;

/**
 * @brief The NeuronGeometryBuilder class transform a Morphology object into primitive geometry.
 * @tparam PrimitiveType The type of primitive that forms the geometry to build.
 */
template<typename PrimitiveType>
class NeuronGeometryBuilder
{
public:
    static_assert(NeuronBuilderNotSpecialized<PrimitiveType>, "NeuronGeometryBuilder not specialized");

    /**
     * @brief Builds the geometry from the given morphology
     * @param morphology The morphology to transform into geometry
     * @returns The built neuron geometry object
     */
    static NeuronGeometry<PrimitiveType> build(const NeuronMorphology &morphology)
    {
        (void)morphology;
        return {};
    }
};

/**
 * @brief Instantiates a neuron geometry object based on a given transformation.
 * @tparam PrimitiveType Type of primitive that forms the geometry to be instantiated
 */
template<typename PrimitiveType>
class NeuronGeometryInstantiator
{
public:
    static_assert(NeuronBuilderNotSpecialized<PrimitiveType>, "NeuronGeometryInstantiator::instantiate not specialized");

    /**
     * @brief Instantiates the neuron geometry with the given transform.
     * @param position Translation A translation in a vector form.
     * @param rotation Rotation A rotation in quaternion form.
     * @return NeuronGeometry<PrimitiveType> the geometry instantiated with the given transformation.
     */
    static NeuronGeometry<PrimitiveType> instantiate(
        const NeuronGeometry<PrimitiveType> &source,
        const brayns::Vector3f &translation,
        const brayns::Quaternion &rotation)
    {
        (void)source;
        (void)translation;
        (void)rotation;
        return {};
    }
};
