/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#include <brayns/utils/EnumUtils.h>

/**
 * @brief Methods availables to color a neuron circuit by (if corresponding data is available as well)
 */
enum class NeuronColorMethods
{
    BY_ETYPE,
    BY_MTYPE,
    BY_LAYER,
    BY_REGION,
    BY_HEMISPHERE,
    BY_MORPHOLOGY,
    BY_MORPHOLOGY_CLASS,
    BY_MORPHOLOGY_SECTION,
    BY_SYNAPSE_CLASS
};

namespace brayns
{
template<>
inline std::vector<std::pair<std::string, NeuronColorMethods>> enumMap()
{
    return {
        {"etype", NeuronColorMethods::BY_ETYPE},
        {"mtype", NeuronColorMethods::BY_MTYPE},
        {"layer", NeuronColorMethods::BY_LAYER},
        {"region", NeuronColorMethods::BY_REGION},
        {"hemisphere", NeuronColorMethods::BY_HEMISPHERE},
        {"morphology", NeuronColorMethods::BY_MORPHOLOGY},
        {"morphology class", NeuronColorMethods::BY_MORPHOLOGY_CLASS},
        {"morphology section", NeuronColorMethods::BY_MORPHOLOGY_SECTION},
        {"synapse class", NeuronColorMethods::BY_SYNAPSE_CLASS}};
}
}
