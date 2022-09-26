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

#include <api/reports/ReportMapping.h>
#include <api/reports/indexers/OffsetIndexer.h>
#include <api/reports/indexers/SpikeIndexer.h>
#include <components/ReportData.h>
#include <io/sonataloader/data/SonataSimulationMapping.h>
#include <io/sonataloader/reports/SonataReportData.h>
#include <io/sonataloader/reports/SonataSpikeData.h>
#include <systems/ReportSystem.h>

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
}

namespace sonataloader
{
void NeuronReportFactory::create(NodeLoadContext &ctxt, const std::vector<CellCompartments> &compartments)
{
    const auto &params = ctxt.params;
    const auto reportType = params.report_type;

    if (reportType == ReportType::None)
    {
        return;
    }

    auto &cb = ctxt.progress;
    const auto &network = ctxt.config;
    const auto &simConfig = network.simulationConfig();
    const auto &population = ctxt.population;
    const auto populationName = population.name();
    const auto &selection = ctxt.selection;

    std::unique_ptr<IReportData> data;
    std::unique_ptr<IColormapIndexer> indexer;

    if (reportType == ReportType::Spikes)
    {
        cb.update("Loading spikes");

        const auto path = SonataConfig::resolveSpikesPath(simConfig);
        auto spikeTransition = params.spike_transition_time;
        data = std::make_unique<SonataSpikeData>(path, populationName, selection, spikeTransition);
        indexer = std::make_unique<SpikeIndexer>(compartments);
    }
    else
    {
        const auto &reportName = params.report_name;
        cb.update("Loading report " + reportName);

        const auto path = sl::SonataConfig::resolveReportPath(simConfig, reportName);
        data = std::make_unique<SonataReportData>(path, populationName, selection);

        const auto flatSelection = selection.flatten();
        const auto reportMapping = SonataCompartmentMapping::generate(path, populationName, flatSelection);
        indexer = std::make_unique<OffsetIndexer>(compartments, reportMapping);
    }

    auto &model = ctxt.model;

    auto &components = model.getComponents();
    components.add<ReportData>(std::move(data), std::move(indexer));

    auto &systems = model.getSystems();
    systems.setPreRenderSystem<ReportSystem>();
}
}
