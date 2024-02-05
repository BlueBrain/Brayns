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

#include <plugin/io/morphology/neuron/NeuronBuilder.h>

/**
 * @brief The SDFNeuronBuilder class is a builder that transform a Morphology
 *        object into SDF (Signed distance field) geometry.
 */
class SDFNeuronBuilder : public NeuronBuilder
{
public:
    SDFNeuronBuilder()
        : NeuronBuilder("smooth")
    {
    }

    NeuronInstantiableGeometry::Ptr _buildImpl(
        const NeuronMorphology&) const final;
};
