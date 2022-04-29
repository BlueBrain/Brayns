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

#include "ReportLoader.h"

#include <brayns/common/Log.h>

#include <api/reports/ReportMapping.h>
#include <api/reports/indexers/OffsetIndexer.h>
#include <api/reports/indexers/SpikeIndexer.h>

#include <components/ReportComponent.h>

#include <io/bbploader/reports/CompartmentData.h>
#include <io/bbploader/reports/SpikeData.h>

namespace bbploader
{
void ReportLoader::load(
    const LoadContext &context,
    const std::vector<CellCompartments> &compartments,
    ProgressUpdater &callback,
    brayns::Model &model)
{
    const auto &params = context.loadParameters;
    const auto reportType = params.report_type;
    if (reportType == ReportType::NONE)
    {
        return;
    }

    const auto &config = context.config;
    const auto &gids = context.gids;

    std::unique_ptr<IReportData> data;
    std::unique_ptr<IColormapIndexer> indexer;

    if (reportType == bbploader::ReportType::COMPARTMENT)
    {
        const auto &reportName = params.report_name;
        callback.update("Loading report " + reportName);

        const auto reportPath = config.getReportSource(reportName).getPath();
        const brion::URI uri(reportPath);
        auto report = std::make_unique<brion::CompartmentReport>(uri, brion::AccessMode::MODE_READ, gids);
        data = std::make_unique<CompartmentData>(std::move(report));

        CompartmentData &compartmentData = static_cast<CompartmentData &>(*data);
        const auto reportCompartments = compartmentData.computeMapping();
        auto offsets = CompartmentMappingGenerator::generate(compartments, reportCompartments);
        indexer = std::make_unique<OffsetIndexer>(std::move(offsets));
    }
    else if (reportType == bbploader::ReportType::SPIKES)
    {
        callback.update("Loading spikes");

        const auto reportPath = config.getSpikeSource().getPath();
        const brion::URI uri(reportPath);
        auto report = std::make_unique<brain::SpikeReportReader>(uri);
        auto spikeTransition = params.spike_transition_time;
        const std::vector<uint64_t> flatGids(gids.begin(), gids.end());
        data = std::make_unique<SpikeData>(std::move(report), flatGids, spikeTransition);

        indexer = std::make_unique<SpikeIndexer>();
    }

    model.addComponent<ReportComponent>(std::move(data), std::move(indexer));
}
}
