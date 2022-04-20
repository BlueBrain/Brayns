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

#include "SynapseImporter.h"

#include <brayns/engine/geometries/Sphere.h>

#include <api/reports/ReportMapping.h>
#include <api/reports/indexers/OffsetIndexer.h>
#include <api/synapse/SynapseColorHandler.h>
#include <components/CircuitColorComponent.h>
#include <components/ReportComponent.h>
#include <components/SynapseComponent.h>
#include <io/sonataloader/colordata/edge/CommonEdgeColorData.h>
#include <io/sonataloader/data/SonataConfig.h>
#include <io/sonataloader/data/SonataSimulationMapping.h>
#include <io/sonataloader/data/SonataSynapses.h>
#include <io/sonataloader/reports/SonataReportData.h>

namespace
{
namespace sl = sonataloader;

struct SynapseReportMapping
{
    static std::unordered_map<uint64_t, size_t> generate(
        const std::string &reportPath,
        const std::string &population,
        const bbp::sonata::Selection &nodeSelection)
    {
        const auto nodeIds = nodeSelection.flatten();
        const auto rawMapping = sl::SonataSimulationMapping::getCompartmentMapping(reportPath, population, nodeIds);

        std::unordered_map<uint64_t, size_t> mapping;
        size_t offset = 0;
        for (const auto &[nodeId, synapseId] : rawMapping)
        {
            mapping[synapseId] = offset++;
        }

        return mapping;
    }
};

struct SynapseReportImporter
{
    static void import(sl::EdgeLoadContext &ctxt, std::vector<uint64_t> orderedEdgeIds)
    {
        const auto &params = ctxt.params;
        const auto &reportName = params.edge_report_name;

        if (reportName.empty())
        {
            return;
        }

        const auto &edgePopulation = ctxt.edgePopulation;
        const auto edgePopulationName = edgePopulation.name();
        const auto &nodeSelection = ctxt.nodeSelection;
        const auto &network = ctxt.config;
        const auto &simConfig = network.simulationConfig();
        const auto path = sl::SonataConfig::resolveReportPath(simConfig, reportName);

        auto data = std::make_unique<sl::SonataReportData>(path, edgePopulationName, nodeSelection);

        const auto mapping = SynapseReportMapping::generate(path, edgePopulationName, nodeSelection);
        auto offsets = ElementMappingGenerator::generate(orderedEdgeIds, mapping);
        auto indexer = std::make_unique<OffsetIndexer>(std::move(offsets));

        auto &model = ctxt.model;
        model.addComponent<ReportComponent>(std::move(data), std::move(indexer));
    }
};

/**
 * @brief Generates the synapse geometry structure and a flat array where the synapse
 * IDs are ordered in sync with their global index in the geometry, needed to properly
 * create the synapse report mapping
 */
struct SynapseAppender
{
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
            geometryList.shrink_to_fit();
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
void SynapseImporter::fromContext(EdgeLoadContext &ctxt)
{
    const auto &edgeSelection = ctxt.edgeSelection;
    const auto edgeIds = edgeSelection.flatten();
    const auto &population = ctxt.edgePopulation;
    const auto &params = ctxt.params;
    const auto afferent = params.load_afferent;

    std::vector<uint64_t> srcNodes;
    std::vector<brayns::Vector3f> surfacePos;

    if (afferent)
    {
        srcNodes = SonataSynapses::getTargetNodes(population, edgeSelection);
        surfacePos = SonataSynapses::getAfferentSurfacePos(population, edgeSelection);
    }
    else
    {
        srcNodes = SonataSynapses::getSourceNodes(population, edgeSelection);
        surfacePos = SonataSynapses::getEfferentSurfacePos(population, edgeSelection);
    }

    fromData(ctxt, srcNodes, surfacePos);
}

void SynapseImporter::fromData(
    EdgeLoadContext &ctxt,
    const std::vector<uint64_t> &nodeIds,
    const std::vector<brayns::Vector3f> &positions)
{
    const auto &params = ctxt.params;
    const auto radius = params.radius;
    const auto afferent = params.load_afferent;
    auto &model = ctxt.model;
    const auto &network = ctxt.config;
    const auto &config = network.circuitConfig();
    const auto &edgePopulation = ctxt.edgePopulation;
    const auto targetPopulationName = afferent ? edgePopulation.target() : edgePopulation.source();
    const auto &edgeSelection = ctxt.edgeSelection;
    const auto flatEdgeIds = edgeSelection.flatten();

    // Geometry
    SynapseAppender appender(flatEdgeIds, nodeIds, positions, radius);
    auto &synapseGeometry = appender.geometry;
    auto &orderedEdgeIds = appender.orderedSynapseIds;

    auto &synapses = model.addComponent<SynapseComponent>();
    synapses.addSynapses(synapseGeometry);

    // Coloring
    auto colorHandler = std::make_unique<SynapseColorHandler>(synapses);
    auto colorData = std::make_unique<CommonEdgeColorData>(config.getNodePopulation(targetPopulationName));
    model.addComponent<CircuitColorComponent>(std::move(colorData), std::move(colorHandler));

    // Simulation
    SynapseReportImporter::import(ctxt, orderedEdgeIds);
}
}
