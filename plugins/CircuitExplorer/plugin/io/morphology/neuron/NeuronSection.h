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

#include <cstdint>

#include <brayns/utils/EnumUtils.h>

/**
 * @brief The NeuronSection enum list holds all the available neuron/astrocyte
 * morphology sections
 */
enum class NeuronSection : uint8_t
{
    SOMA = 1,
    AXON = 2,
    DENDRITE = 3,
    APICAL_DENDRITE = 4
};

namespace brayns
{
template <>
inline std::vector<std::pair<std::string, NeuronSection>> enumMap()
{
    return {{"soma", NeuronSection::SOMA},
            {"axon", NeuronSection::AXON},
            {"dendrite", NeuronSection::DENDRITE},
            {"apical_dendrite", NeuronSection::APICAL_DENDRITE}};
}
} // namespace brayns
