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

#include <brain/spikeReportReader.h>

#include <unordered_map>

namespace dti
{
class SpikeReportComponent final : public brayns::Component
{
public:
    /**
     * @brief Construct a new Spike Report Component object
     *
     * @param report
     * @param gidStreamlineMap
     * @param spikeDecayTIme
     */
    SpikeReportComponent(
        std::unique_ptr<brain::SpikeReportReader> report,
        std::unordered_map<uint64_t, std::vector<size_t>> gidStreamlineMap,
        float spikeDecayTime);

    void onCreate() override;

    void onPreRender(const brayns::ParametersManager &parameters) override;

private:
    const std::unique_ptr<brain::SpikeReportReader> _report;
    const std::unordered_map<uint64_t, std::vector<size_t>> _gidStreamlineMap;
    const float _invSpikeDecayTime{};

    // Flag used to force the simulations color update when re-enabling a simulation after it was disabled
    bool _lastEnabledValue{true};
};
}
