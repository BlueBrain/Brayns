/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

/**
 * @brief The NeuronGeometryBuilder class transform a Morphology object into primitive geometry
 */
class NeuronGeometryBuilder
{
public:
    /**
     * @brief Builds and stores the geometry from the given morphology
     * @param morphology
     */
    explicit NeuronGeometryBuilder(const NeuronMorphology &morphology);

    /**
     * @brief Instantiates the built geometry with the given transform
     * @param position (Translation)
     * @param rotation (Rotation)
     * @return NeuronGeometry
     */
    NeuronGeometry instantiate(const brayns::Vector3f &t, const brayns::Quaternion &r) const;

private:
    NeuronGeometry _data;
};
