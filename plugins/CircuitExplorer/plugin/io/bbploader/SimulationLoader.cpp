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

#include "SimulationLoader.h"

#include <brayns/common/Log.h>

#include <plugin/io/bbploader/simulation/CompartmentReportComponent.h>
#include <plugin/io/bbploader/simulation/SpikeReportComponent.h>

namespace bbploader
{
void SimulationLoader::load(const brion::BlueConfig &config,
                            const BBPLoaderParameters &params,
                            const brain::GIDSet& gids,
                            const std::vector<CompartmentStructure> &compartments,
                            brayns::Model &model)
{
    const auto reportType = params.report_type;
    if (reportType == bbploader::SimulationType::COMPARTMENT)
    {
        const auto &reportName = params.report_name;
        const auto reportPath = config.getReportSource(reportName).getPath();
        const brion::URI uri (reportPath);
        auto report = std::make_unique<brion::CompartmentReport>(uri, brion::AccessMode::MODE_READ, gids);
        model.addComponent<CompartmentReportComponent>(std::move(report), compartments);
    }
    else if(reportType == bbploader::SimulationType::SPIKES)
    {
        const auto reportPath = config.getSpikeSource().getPath();
        const brion::URI uri (reportPath);
        auto report = std::make_unique<brion::SpikeReport>(uri, brion::AccessMode::MODE_READ);
        auto spikeTransition = params.spike_transition_time;
        model.addComponent<SpikeReportComponent>(std::move(report), gids, spikeTransition);
    }
}
}
