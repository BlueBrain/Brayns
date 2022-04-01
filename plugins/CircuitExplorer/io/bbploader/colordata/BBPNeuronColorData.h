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

#include <api/IColorData.h>

namespace bbploader
{
class BBPNeuronColorData final : public IColorData
{
public:
    /**
     * @brief BBPNeuronColorData
     * @param circuitPath
     * @param circuitPop
     */
    BBPNeuronColorData(std::string circuitPath, std::string circuitPop);

    /**
     * @brief getCircuitMethods
     * @return
     */
    std::vector<std::string> getMethods() const noexcept override;

    /**
     * @brief getCircuitMethodVariables
     * @param method
     * @return
     */
    std::vector<std::string> getMethodVariables(const std::string &method) const override;

    /**
     * @brief getMethodValuesForIDs
     * @param method
     * @param ids
     * @return
     */
    std::vector<std::string> getMethodValuesForIDs(const std::string &method,
                                                   const std::vector<uint64_t>& ids) const override;

    /**
     * @brief Returns the available coloring methods at the circuit level
     * @return std::vector<std::string>
     */
    std::vector<std::string> getCircuitLevelMethods() const;

    /**
     * @brief Returns the available variables for the given circuit - level method
     * @param method
     * @return std::vector<std::string>
     */
    std::vector<std::string> getCircuitLevelMethodVariables(const std::string &method) const;

private:
    const std::string _circuitPath;
    // Circuit population (for bbp - sonata hybird circuits)
    const std::string _circuitPop;
};
} // namespace bbploader
