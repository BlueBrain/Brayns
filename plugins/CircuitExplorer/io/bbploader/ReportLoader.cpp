/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <api/reports/ReportFactory.h>
#include <api/reports/indexers/OffsetIndexer.h>
#include <api/reports/indexers/SpikeIndexer.h>
#include <io/bbploader/reports/CompartmentData.h>
#include <io/bbploader/reports/SpikeData.h>

namespace
{
class IHandler
{
public:
    virtual ~IHandler() = default;
    virtual ReportData createData() = 0;
};

class CompartmentHandler : public IHandler
{
public:
    CompartmentHandler(const bbploader::LoadContext &context, const std::vector<CellCompartments> &compartments):
        _context(context),
        _compartments(compartments)
    {
    }

    ReportData createData() override
    {
        auto reportData = ReportData();
        reportData.data = _createData();
        reportData.indexer = _createIndexer(reportData.data);
        return reportData;
    }

private:
    std::unique_ptr<IReportData> _createData()
    {
        auto &params = _context.loadParameters;
        auto &config = _context.config;
        auto &gids = _context.gids;

        auto &reportName = params.report_name;
        auto reportPath = config.getReportSource(reportName).getPath();
        auto uri = brion::URI(reportPath);
        auto report = std::make_unique<brion::CompartmentReport>(uri, brion::AccessMode::MODE_READ, gids);

        return std::make_unique<bbploader::CompartmentData>(std::move(report));
    }

    std::unique_ptr<IColormapIndexer> _createIndexer(std::unique_ptr<IReportData> &data)
    {
        auto &compartmentData = static_cast<bbploader::CompartmentData &>(*data);
        const auto reportCompartments = compartmentData.computeMapping();
        return std::make_unique<OffsetIndexer>(_compartments, reportCompartments);
    }

private:
    const bbploader::LoadContext &_context;
    const std::vector<CellCompartments> &_compartments;
};

class SpikeHandler : public IHandler
{
public:
    SpikeHandler(const bbploader::LoadContext &context, const std::vector<CellCompartments> &compartments):
        _context(context),
        _compartments(compartments)
    {
    }

    ReportData createData() override
    {
        auto reportData = ReportData();
        reportData.data = _createData();
        reportData.indexer = _createIndexer();
        return reportData;
    }

private:
    std::unique_ptr<IReportData> _createData()
    {
        auto &config = _context.config;
        auto &params = _context.loadParameters;
        auto &gids = _context.gids;

        auto reportPath = config.getSpikeSource().getPath();
        auto uri = brion::URI(reportPath);
        auto report = std::make_unique<brain::SpikeReportReader>(uri);

        auto spikeTransition = params.spike_transition_time;

        const std::vector<uint64_t> flatGids(gids.begin(), gids.end());
        return std::make_unique<bbploader::SpikeData>(std::move(report), flatGids, spikeTransition);
    }

    std::unique_ptr<IColormapIndexer> _createIndexer()
    {
        return std::make_unique<SpikeIndexer>(_compartments);
    }

private:
    const bbploader::LoadContext &_context;
    const std::vector<CellCompartments> &_compartments;
};

class ReportHandlerFactory
{
public:
    static std::unique_ptr<IHandler> createHandler(
        const bbploader::LoadContext &context,
        const std::vector<CellCompartments> &compartments,
        ProgressUpdater &callback)
    {
        auto &params = context.loadParameters;
        auto reportType = params.report_type;

        if (reportType == bbploader::ReportType::None)
        {
            return {};
        }

        callback.update();

        if (reportType == bbploader::ReportType::Compartment)
        {
            return std::make_unique<CompartmentHandler>(context, compartments);
        }
        return std::make_unique<SpikeHandler>(context, compartments);
    }
};
}

namespace bbploader
{
void ReportLoader::load(
    const LoadContext &context,
    const std::vector<CellCompartments> &compartments,
    ProgressUpdater &callback,
    brayns::Model &model)
{
    auto handler = ReportHandlerFactory::createHandler(context, compartments, callback);
    if (!handler)
    {
        return;
    }
    auto reportData = handler->createData();
    ReportFactory::create(model, std::move(reportData));
}
}
