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

#include <api/neuron/NeuronBuilder.h>

#include <brayns/engine/geometry/types/Sphere.h>

template<>
class NeuronGeometryBuilder<brayns::Sphere>
{
public:
    static NeuronGeometry<brayns::Sphere> build(const NeuronMorphology &morphology);
};

template<>
class NeuronGeometryInstantiator<brayns::Sphere>
{
public:
    static NeuronGeometry<brayns::Sphere> instantiate(
        const NeuronGeometry<brayns::Sphere> &source,
        const brayns::Vector3f &translation,
        const brayns::Quaternion &rotation);
};
