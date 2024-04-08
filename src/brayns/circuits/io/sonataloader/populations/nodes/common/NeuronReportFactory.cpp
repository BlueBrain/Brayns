/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/circuits/api/reports/ReportFactory.h>
#include <brayns/circuits/api/reports/ReportMapping.h>
#include <brayns/circuits/api/reports/indexers/OffsetIndexer.h>
#include <brayns/circuits/api/reports/indexers/SpikeIndexer.h>
#include <brayns/circuits/io/sonataloader/data/SimulationMapping.h>
#include <brayns/circuits/io/sonataloader/reports/SonataReportData.h>
#include <brayns/circuits/io/sonataloader/reports/SonataSpikeData.h>

namespace
{
namespace sl = sonataloader;

class SonataCompartmentMapping
{
public:
    static std::vector<CellReportMapping> generate(
        const std::string &path,
        const std::string &population,
        const std::vector<uint64_t> &nodeList)
    {
        auto compartmentsSize = _computeCompartmentsSize(path, population, nodeList);
        return _computeMapping(compartmentsSize);
    }

private:
    static std::vector<std::vector<uint16_t>> _computeCompartmentsSize(
        const std::string &reportPath,
        const std::string &population,
        const std::vector<uint64_t> &nodeList)
    {
        auto compartments = sl::SimulationMapping::getCompartmentMapping(reportPath, population, nodeList);
        auto indexer = _createIndexer(nodeList);
        auto compartmentsSize = _allocateCompartmentsSize(nodeList.size(), compartments, indexer);

        auto lastSection = std::numeric_limits<uint32_t>::max();
        auto lastNode = std::numeric_limits<uint64_t>::max();

        for (auto &key : compartments)
        {
            auto nodeId = key[0];
            auto elementId = key[1];
            auto index = indexer[nodeId];

            auto &cm = compartmentsSize[index];

            if (lastSection != elementId || lastNode != nodeId)
            {
                lastNode = nodeId;
                lastSection = static_cast<uint32_t>(elementId);
            }

            cm[elementId]++;
        }

        return compartmentsSize;
    }

    static std::vector<size_t> _createIndexer(const std::vector<uint64_t> &nodeList)
    {
        auto indexer = std::vector<size_t>(nodeList.back() + 1, std::numeric_limits<size_t>::max());
        for (size_t i = 0; i < nodeList.size(); ++i)
        {
            indexer[nodeList[i]] = i;
        }
        return indexer;
    }

    static std::vector<std::vector<uint16_t>> _allocateCompartmentsSize(
        size_t numNodes,
        const std::vector<bbp::sonata::CompartmentID> &compartments,
        const std::vector<size_t> &indexer)
    {
        auto highestSections = _perNodeHighestSection(numNodes, compartments, indexer);
        auto compartmentsSize = std::vector<std::vector<uint16_t>>(highestSections.size());

        for (size_t i = 0; i < compartmentsSize.size(); ++i)
        {
            auto highestSection = highestSections[i];
            compartmentsSize[i].resize(highestSection + 1);
        }

        return compartmentsSize;
    };

    static std::vector<size_t> _perNodeHighestSection(
        size_t numNodes,
        const std::vector<bbp::sonata::CompartmentID> &comparments,
        const std::vector<size_t> &indexer)
    {
        auto result = std::vector<size_t>(numNodes, 0ul);

        for (auto &compartment : comparments)
        {
            auto nodeId = compartment[0];
            auto sectionId = compartment[1];
            auto &highest = result[indexer[nodeId]];
            highest = std::max(highest, sectionId);
        }

        return result;
    }

    static std::vector<CellReportMapping> _computeMapping(const std::vector<std::vector<uint16_t>> &compartmentSizes)
    {
        auto mapping = std::vector<CellReportMapping>();
        mapping.reserve(compartmentSizes.size());

        auto prevOffset = 0ul;

        for (auto &sizes : compartmentSizes)
        {
            auto &cellMapping = mapping.emplace_back();
            cellMapping.globalOffset = prevOffset;
            cellMapping.compartments.reserve(sizes.size());
            cellMapping.offsets.reserve(sizes.size());

            uint16_t localOffset = 0;
            for (auto size : sizes)
            {
                cellMapping.offsets.push_back(localOffset);
                localOffset = static_cast<uint16_t>(localOffset + size);
                cellMapping.compartments.push_back(size);
            }

            prevOffset += localOffset;
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
        auto &config = context.config;
        auto &params = context.params;
        auto &reportName = params.report_name;
        return config.getReportPath(reportName);
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
        auto &config = context.config;
        auto path = config.getSpikesPath();
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
        callback.update();

        auto &params = context.params;
        auto reportType = params.report_type;
        if (reportType == sl::ReportType::Spikes)
        {
            return SpikeReportData::create(context, compartments);
        }
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
