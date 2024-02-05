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

#include <bbp/sonata/common.h>
#include <bbp/sonata/config.h>

namespace sonataloader
{
/**
 * @brief The NeuronColorHandler class provides functionality
 *        to set a biophysical and astrocyte node population circuit color
 */
class SonataNeuronColorHandler
    : public NeuronColorHandler<std::vector<bbp::sonata::NodeID>>
{
public:
    SonataNeuronColorHandler(const std::string& configPath,
                             const std::string& population);

    std::vector<std::string> _getExtraMethods() const final;
    std::vector<std::string> _getValuesForMethod(
        const std::string& method) const final;

private:
    const bbp::sonata::CircuitConfig _config;
    const std::string _population;
};
} // namespace sonataloader
