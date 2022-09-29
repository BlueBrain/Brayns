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

#include "NeuronReportFactory.h"

#include <api/reports/ReportFactory.h>
#include <api/reports/ReportMapping.h>
#include <api/reports/indexers/OffsetIndexer.h>
#include <api/reports/indexers/SpikeIndexer.h>
#include <io/sonataloader/data/SonataSimulationMapping.h>
#include <io/sonataloader/reports/SonataReportData.h>
#include <io/sonataloader/reports/SonataSpikeData.h>

namespace
{
namespace sl = sonataloader;

struct SonataCompartmentMapping
{
    static std::vector<CellReportMapping>
        generate(const std::string &reportPath, const std::string &population, const std::vector<uint64_t> &nodeList)
    {
        const auto rawMapping = sl::SonataSimulationMapping::getCompartmentMapping(reportPath, population, nodeList);

        // Compact mapping
        std::map<uint64_t, std::vector<uint16_t>> sortedCompartmentsSize;
        uint32_t lastSection = std::numeric_limits<uint32_t>::max();
        uint64_t lastNode = std::numeric_limits<uint64_t>::max();
        for (const auto &key : rawMapping)
        {
            auto &cm = sortedCompartmentsSize[key.first];
            if (lastSection != key.second || lastNode != key.first)
            {
                lastNode = key.first;
                lastSection = key.second;
                cm.push_back(0u);
            }
            cm[key.second]++;
        }

        // Returns a node id sorted list of compartment mappings
        std::vector<CellReportMapping> mapping(sortedCompartmentsSize.size());
        // Transform into brayns mapping
        auto it = sortedCompartmentsSize.begin();
        size_t index = 0;
        size_t prevOffset = 0;
        for (; it != sortedCompartmentsSize.end(); ++it)
        {
            auto &cellMapping = mapping[index];
            cellMapping.globalOffset = prevOffset;
            cellMapping.compartments.resize(it->second.size());
            cellMapping.offsets.resize(it->second.size());

            uint16_t localOffset = 0;
            for (size_t i = 0; i < it->second.size(); ++i)
            {
                const auto sectionCompartments = it->second[i];
                cellMapping.offsets[i] = localOffset;
                cellMapping.compartments[i] = sectionCompartments;
                localOffset += sectionCompartments;
                prevOffset += sectionCompartments;
            }

            ++index;
        }

        return mapping;
    }
};

class CompartmentReportData
{
public:
    static ReportData create(const sl::NodeLoadContext &context, const std::vector<CellCompartments> &compartments)
    {
        auto path = resolveCompartmentPath(context);
        auto reportData = ReportData();
        reportData.data = _createData(context, path);
        reportData.indexer = _createIndexer(context, compartments, path);
        return reportData;
    }

private:
    static std::string resolveCompartmentPath(const sl::NodeLoadContext &context)
    {
        auto &simConfig = context.config.simulationConfig();
        auto &params = context.params;
        auto &reportName = params.report_name;
        return sl::SonataConfig::resolveReportPath(simConfig, reportName).string();
    }

    static std::unique_ptr<IReportData> _createData(const sl::NodeLoadContext &context, const std::string &path)
    {
        auto &params = context.params;
        auto &populationName = params.node_population;
        auto &selection = context.selection;
        return std::make_unique<sl::SonataReportData>(path, populationName, selection);
    }

    static std::unique_ptr<IColormapIndexer> _createIndexer(
        const sl::NodeLoadContext &context,
        const std::vector<CellCompartments> &compartments,
        const std::string &path)
    {
        auto &params = context.params;
        auto &populationName = params.node_population;
        auto &selection = context.selection;
        auto flatSelection = selection.flatten();
        auto reportMapping = SonataCompartmentMapping::generate(path, populationName, flatSelection);
        return std::make_unique<OffsetIndexer>(compartments, reportMapping);
    }
};

class SpikeReportData
{
public:
    static ReportData create(const sl::NodeLoadContext &context, const std::vector<CellCompartments> &compartments)
    {
        auto reportData = ReportData();
        reportData.data = _createData(context);
        reportData.indexer = _createIndexer(compartments);
        return reportData;
    }

private:
    static std::unique_ptr<IReportData> _createData(const sl::NodeLoadContext &context)
    {
        auto &simConfig = context.config.simulationConfig();
        auto path = sl::SonataConfig::resolveSpikesPath(simConfig);
        auto &params = context.params;
        auto &population = params.node_population;
        auto &selection = context.selection;
        auto spikeTransition = params.spike_transition_time;
        return std::make_unique<sl::SonataSpikeData>(path, population, selection, spikeTransition);
    }

    static std::unique_ptr<IColormapIndexer> _createIndexer(const std::vector<CellCompartments> &compartments)
    {
        return std::make_unique<SpikeIndexer>(compartments);
    }
};

class ReportHandler
{
public:
    static bool hasReport(const sl::NodeLoadContext &context)
    {
        auto &params = context.params;
        auto reportType = params.report_type;
        return reportType != sl::ReportType::None;
    }

    static ReportData createReportData(
        const sl::NodeLoadContext &context,
        const std::vector<CellCompartments> &compartments)
    {
        auto &callback = context.progress;

        auto &params = context.params;
        auto reportType = params.report_type;
        if (reportType == sl::ReportType::Spikes)
        {
            callback.update("Loading spikes");
            return SpikeReportData::create(context, compartments);
        }

        callback.update("Loading compartment report");
        return CompartmentReportData::create(context, compartments);
    }
};
}

namespace sonataloader
{
void NeuronReportFactory::create(NodeLoadContext &context, const std::vector<CellCompartments> &compartments)
{
    if (!ReportHandler::hasReport(context))
    {
        return;
    }

    auto reportData = ReportHandler::createReportData(context, compartments);
    ReportFactory::create(context.model, std::move(reportData));
}
}
