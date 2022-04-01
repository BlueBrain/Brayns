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

#include <io/bbploader/simulation/CompartmentReportComponent.h>
#include <io/bbploader/simulation/SpikeReportComponent.h>

namespace bbploader
{
void SimulationLoader::load(
        const LoadContext& ctxt, const std::vector<CompartmentStructure> &comparts, brayns::Model &model)
{
    const auto &params = ctxt.loadParameters;
    const auto &config = ctxt.config;
    const auto &gids = ctxt.gids;

    const auto reportType = params.report_type;
    if (reportType == bbploader::SimulationType::COMPARTMENT)
    {
        const auto &reportName = params.report_name;
        const auto reportPath = config.getReportSource(reportName).getPath();
        const brion::URI uri (reportPath);
        auto report = std::make_unique<brion::CompartmentReport>(uri, brion::AccessMode::MODE_READ, gids);
        model.addComponent<CompartmentReportComponent>(std::move(report), comparts);
    }
    else if(reportType == bbploader::SimulationType::SPIKES)
    {
        const auto reportPath = config.getSpikeSource().getPath();
        const brion::URI uri (reportPath);
        auto report = std::make_unique<brain::SpikeReportReader>(uri);
        auto spikeTransition = params.spike_transition_time;
        model.addComponent<SpikeReportComponent>(std::move(report), gids, spikeTransition);
    }
}
}
