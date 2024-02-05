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

#include <plugin/io/morphology/neuron/NeuronColorHandler.h>

#include <brain/brain.h>

namespace bbploader
{
/**
 * @brief The BBPNeuronColorHandler class handlers element coloring for neuron
 * and astrocytes that were loaded from BBP format files
 * (CircuitConfig/BlueConfig)
 */
class BBPNeuronColorHandler : public NeuronColorHandler<brion::GIDSet>
{
public:
    BBPNeuronColorHandler(const std::string& circuitPath,
                          const std::string& circuitPop);

    std::vector<std::string> _getExtraMethods() const final;
    std::vector<std::string> _getValuesForMethod(
        const std::string& method) const final;

private:
    const std::string _circuitPath;
    // Circuit population (for bbp - sonata hybird circuits)
    const std::string _circuitPop;
};
} // namespace bbploader
