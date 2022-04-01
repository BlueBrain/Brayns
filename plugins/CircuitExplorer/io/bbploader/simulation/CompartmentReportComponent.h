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

#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/common/DataHandler.h>

#include <io/simulation/SimulationMapping.h>

#include <brion/compartmentReport.h>

namespace bbploader
{
class CompartmentReportComponent final : public brayns::Component
{
public:
    /**
     * @brief Creates a compartment report component from the compartment report reader and the geometry  compartment
     * mapping needed to create the mapping between simulation frame and geometry
     * @param report
     * @param compartments
     */
    CompartmentReportComponent(std::unique_ptr<brion::CompartmentReport> report,
                               const std::vector<CompartmentStructure> &compartments);

    /**
     * @brief getSizeInBytes implementation
     * @return
     */
    size_t getSizeInBytes() const noexcept override;

    /**
     * @brief Initializes the transfer function used in the simulation, and adds a SimulationComponent to the model
     * to access and control the simulation
     */
    void onStart() override;

    /**
     * @brief Rebuilds (if needed) the color buffer from the transfer function and load (if needed) a new
     * frame from the report. If any of the previous actions happened, will update the colors in the circuit.
     * Will have no action if the simulation is disabled for this model
     * @param parameters
     */
    void onPreRender(const brayns::ParametersManager &parameters) override;

private:
    const std::unique_ptr<brion::CompartmentReport> _report;
    const std::vector<uint64_t> _offsets;

    brayns::OSPBuffer _colors;
    std::vector<uint8_t> _indices;

    // Flag used to force the simulations color update when re-enabling a simulation after it was disabled
    bool _lastEnabledValue {true};
};
}
