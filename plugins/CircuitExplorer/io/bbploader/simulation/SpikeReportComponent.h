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

#include <brain/spikeReportReader.h>
#include <brain/types.h>

#include <unordered_map>

namespace bbploader
{
class SpikeReportComponent final : public brayns::Component
{
public:
    /**
     * @brief Creates a Spike report component from the given spike reader, the
     * list of GIDs that will be affected and the rest <-> spiking transition time
     * @param report
     * @param gids
     * @param spikeTimeInterval
     */
    SpikeReportComponent(std::unique_ptr<brain::SpikeReportReader> report,
                         const brain::GIDSet &gids,
                         const float spikeTimeInterval);

    /**
     * @brief getSizeInBytes implementation
     * @return size_t
     */
    size_t getSizeInBytes() const noexcept override;

    /**
     * @brief Initializes the transfer function used by the simulation and adds a SimulationComponent to the model
     * to control and acess the simulation
     */
    void onStart() override;

    /**
     * @brief If simulation is enabled for this model, updates the transfer function buffer, loads a new simulation
     * frame and updates the circuit colors, if needed
     * @param parameters
     */
    void onPreRender(const brayns::ParametersManager &parameters) override;

private:
    std::unique_ptr<brain::SpikeReportReader> _report;
    std::unordered_map<uint32_t, size_t> _gidIndexMap;
    brayns::OSPBuffer _color;
    std::vector<uint8_t> _indices;
    float _spikeInterval {};

    // Flag used to force the simulations color update when re-enabling a simulation after it was disabled
    bool _lastEnabledValue {true};
};
}
