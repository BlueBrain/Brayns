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

#include <plugin/io/morphology/neuron/colorhandlers/NeuronColorData.h>

#include <bbp/sonata/common.h>
#include <bbp/sonata/config.h>

namespace sonataloader
{
/**
 * @brief The NeuronColorHandler class provides functionality to set a biophysical and astrocyte population color
 */
class SonataNeuronColorData final : public NeuronColorData
{
public:
    /**
     * @brief SonataNeuronColorData
     * @param circuitConfig
     * @param population
     */
    SonataNeuronColorData(bbp::sonata::CircuitConfig circuitConfig, const std::string &population);

    /**
     * @brief getCircuitMethods
     * @return
     */
    std::vector<std::string> getCircuitMethods() const noexcept override;

    /**
     * @brief getCircuitMethodVariables
     * @param method
     * @return
     */
    std::vector<std::string> getCircuitMethodVariables(const std::string &method) const override;

    /**
     * @brief getMethodValuesForIDs
     * @param method
     * @param ids
     * @return
     */
    std::vector<std::string> getMethodValuesForIDs(
            const std::string &method, const std::vector<uint64_t> &ids) const override;

private:
    const bbp::sonata::CircuitConfig _config;
    const std::string _population;
};
} // namespace sonataloader
