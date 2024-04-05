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

#include "SynapseImporter.h"

#include "EdgeMetadataFactory.h"

#include <brayns/core/engine/geometry/types/Sphere.h>

#include <brayns/circuit/api/circuit/SynapseCircuitBuilder.h>
#include <brayns/circuit/api/reports/ReportFactory.h>
#include <brayns/circuit/api/reports/ReportMapping.h>
#include <brayns/circuit/api/reports/indexers/OffsetIndexer.h>
#include <brayns/circuit/components/ReportData.h>
#include <brayns/circuit/io/sonataloader/colordata/ColorDataFactory.h>
#include <brayns/circuit/io/sonataloader/data/Config.h>
#include <brayns/circuit/io/sonataloader/data/SimulationMapping.h>
#include <brayns/circuit/io/sonataloader/data/Synapses.h>
#include <brayns/circuit/io/sonataloader/reports/SonataReportData.h>

namespace
{
class SynapseOffsetGenerator
{
public:
    static std::vector<size_t> generate(
        const std::vector<uint64_t> &elementIds,
        const std::unordered_map<uint64_t, size_t> &mapping)
    {
        std::vector<size_t> result(elementIds.size());

        for (size_t i = 0; i < elementIds.size(); ++i)
        {
            auto elementId = elementIds[i];
            auto it = mapping.find(elementId);
            if (it == mapping.end())
            {
                throw std::runtime_error("No report mapping information for element " + std::to_string(elementId));
            }

            auto &entry = *it;
            auto offset = entry.second;
            result[i] = offset;
        }

        return result;
    }
};

namespace sl = sonataloader;

struct SynapseReportMapping
{
    static std::unordered_map<uint64_t, size_t> generate(
        const std::string &reportPath,
        const std::string &population,
        const bbp::sonata::Selection &nodeSelection)
    {
        const auto nodeIds = nodeSelection.flatten();
        const auto rawMapping = sl::SimulationMapping::getCompartmentMapping(reportPath, population, nodeIds);

        std::unordered_map<uint64_t, size_t> mapping;
        size_t offset = 0;
        for (const auto &[nodeId, synapseId] : rawMapping)
        {
            mapping[synapseId] = offset++;
        }

        return mapping;
    }
};

class SynapseReportImporter
{
public:
    static void import(sl::EdgeLoadContext &context, const std::vector<uint64_t> &orderedEdgeIds)
    {
        if (!_hasReport(context))
        {
            return;
        }

        auto reportPath = _getReportFilePath(context);
        auto data = _createReportData(context, reportPath);
        auto indexer = _createReportIndexer(context, reportPath, orderedEdgeIds);
        auto reportData = ReportData{std::move(data), std::move(indexer)};
        auto &model = context.model;
        ReportFactory::create(model, std::move(reportData));
    }

private:
    static bool _hasReport(sl::EdgeLoadContext &context)
    {
        auto &params = context.params;
        auto &reportName = params.edge_report_name;
        return !reportName.empty();
    }

    static std::string _getReportFilePath(sl::EdgeLoadContext &context)
    {
        auto &params = context.params;
        auto &reportName = params.edge_report_name;
        auto &config = context.config;
        return config.getReportPath(reportName);
    }

    static std::unique_ptr<sl::SonataReportData> _createReportData(sl::EdgeLoadContext &context, std::string &path)
    {
        auto &edgePopulation = context.edgePopulation;
        auto edgePopulationName = edgePopulation.name();
        auto &nodeSelection = context.nodeSelection;
        return std::make_unique<sl::SonataReportData>(path, edgePopulationName, nodeSelection);
    }

    static std::unique_ptr<OffsetIndexer> _createReportIndexer(
        sl::EdgeLoadContext &context,
        std::string &path,
        const std::vector<uint64_t> &orderedEdgeIds)
    {
        auto &edgePopulation = context.edgePopulation;
        auto edgePopulationName = edgePopulation.name();
        auto &nodeSelection = context.nodeSelection;
        auto mapping = SynapseReportMapping::generate(path, edgePopulationName, nodeSelection);
        auto offsets = SynapseOffsetGenerator::generate(orderedEdgeIds, mapping);
        return std::make_unique<OffsetIndexer>(std::move(offsets));
    }
};

/**
 * @brief Generates the synapse geometry structure and a flat array where the synapse
 * IDs are ordered in sync with their global index in the geometry, needed to properly
 * create the synapse report mapping
 */
class SynapseAppender
{
public:
    SynapseAppender(
        const std::vector<uint64_t> &edgeIds,
        const std::vector<uint64_t> &nodeIds,
        const std::vector<brayns::Vector3f> &positions,
        float radius)
    {
        struct SynapseGeometryPair
        {
            std::vector<brayns::Sphere> geometry;
            std::vector<uint64_t> synapseIds;
        };

        std::map<uint64_t, SynapseGeometryPair> synapseGeometry;
        for (size_t i = 0; i < nodeIds.size(); ++i)
        {
            const auto nodeId = nodeIds[i];
            auto &buffer = synapseGeometry[nodeId];
            auto &geometry = buffer.geometry;
            auto &edges = buffer.synapseIds;

            auto &position = positions[i];
            geometry.push_back({position, radius});

            auto synapseId = edgeIds[i];
            edges.push_back(synapseId);
        }

        for (auto &[nodeId, buffer] : synapseGeometry)
        {
            auto &geometryList = buffer.geometry;
            auto geometryListCopy = geometryList;
            geometryList = std::move(geometryListCopy);
            geometry[nodeId] = std::move(geometryList);

            auto &edges = buffer.synapseIds;
            auto begin = edges.begin();
            auto end = edges.end();
            auto globalEnd = orderedSynapseIds.end();
            orderedSynapseIds.insert(globalEnd, begin, end);
        }
    }

    std::map<uint64_t, std::vector<brayns::Sphere>> geometry;
    std::vector<uint64_t> orderedSynapseIds;
};
}

namespace sonataloader
{
void SynapseImporter::fromContext(EdgeLoadContext &context)
{
    auto &edgeSelection = context.edgeSelection;
    auto edgeIds = edgeSelection.flatten();
    auto &population = context.edgePopulation;
    auto &params = context.params;
    auto afferent = params.load_afferent;

    if (afferent)
    {
        auto srcNodes = Synapses::getTargetNodes(population, edgeSelection);
        auto surfacePos = Synapses::getAfferentSurfacePos(population, edgeSelection);
        fromData(context, srcNodes, surfacePos);
        return;
    }

    auto srcNodes = Synapses::getSourceNodes(population, edgeSelection);
    auto surfacePos = Synapses::getEfferentSurfacePos(population, edgeSelection);
    fromData(context, srcNodes, surfacePos);
}

void SynapseImporter::fromData(
    EdgeLoadContext &context,
    const std::vector<uint64_t> &nodeIds,
    const std::vector<brayns::Vector3f> &positions)
{
    auto &params = context.params;
    auto radius = params.radius;
    auto &edgeSelection = context.edgeSelection;
    auto flatEdgeIds = edgeSelection.flatten();
    auto appender = SynapseAppender(flatEdgeIds, nodeIds, positions, radius);
    auto &synapseGeometry = appender.geometry;
    auto colorData = ColorDataFactory::create(context);

    auto buildContext = SynapseCircuitBuilder::Context{std::move(synapseGeometry), std::move(colorData)};
    SynapseCircuitBuilder::build(context.model, std::move(buildContext));
    SynapseReportImporter::import(context, appender.orderedSynapseIds);

    EdgeMetadataFactory::create(context);
}
}
