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

#include <plugin/api/CircuitColorHandler.h>

/**
 * @brief The NeuronMaterialMap class its an implementation of the
 * ElementMaterialMap class that allows CircuitColorHandlers to access the
 * materials of a neuron or astrocyte
 */
class NeuronMaterialMap : public ElementMaterialMap
{
public:
    size_t soma{std::numeric_limits<size_t>::max()};
    size_t axon{std::numeric_limits<size_t>::max()};
    size_t dendrite{std::numeric_limits<size_t>::max()};
    size_t apicalDendrite{std::numeric_limits<size_t>::max()};

    void setColor(brayns::ModelDescriptor *model, const brayns::Vector4f &color) final
    {
        _updateMaterial(model, soma, color);
        _updateMaterial(model, axon, color);
        _updateMaterial(model, dendrite, color);
        _updateMaterial(model, apicalDendrite, color);
    }
};
