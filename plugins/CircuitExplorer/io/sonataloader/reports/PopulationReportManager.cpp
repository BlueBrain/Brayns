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

#include "PopulationReportManager.h"

#include <plugin/api/MaterialUtils.h>

#include <plugin/io/sonataloader/reports/handlers/SonataReportHandler.h>
#include <plugin/io/sonataloader/reports/handlers/SonataSpikeHandler.h>
#include <plugin/io/sonataloader/reports/handlers/VasculatureRadiiHandler.h>

#include <plugin/io/sonataloader/reports/loaders/EdgeCompartmentLoader.h>
#include <plugin/io/sonataloader/reports/loaders/NodeCompartmentLoader.h>
#include <plugin/io/sonataloader/reports/loaders/NodeSpikeLoader.h>
#include <plugin/io/sonataloader/reports/loaders/NodeVasculatureReportLoader.h>

#include <plugin/io/util/TransferFunctionUtils.h>

namespace sonataloader
{
namespace
{
NodeReportLoader::Ptr getLoaderForType(const ReportType type)
{
    switch (type)
    {
    case ReportType::BLOODFLOW_PRESSURE:
    case ReportType::BLOODFLOW_RADII:
    case ReportType::BLOODFLOW_SPEED:
        return std::make_unique<NodeVasculatureReportLoader>();
    case ReportType::COMPARTMENT:
    case ReportType::SUMMATION:
        return std::make_unique<NodeCompartmentLoader>();
    case ReportType::SPIKES:
        return std::make_unique<NodeSpikeLoader>();
    default:
        return {nullptr};
    }
}

std::string getReportPath(const ReportType type, const bbp::sonata::SimulationConfig &config, const std::string &name)
{
    switch (type)
    {
    case ReportType::BLOODFLOW_PRESSURE:
    case ReportType::BLOODFLOW_RADII:
    case ReportType::BLOODFLOW_SPEED:
    case ReportType::COMPARTMENT:
    case ReportType::SUMMATION:
    case ReportType::SYNAPSE:
        return SonataConfig::resolveReportPath(config, name);
    case ReportType::SPIKES:
        return SonataConfig::resolveSpikesPath(config);
    case ReportType::NONE:
        return "";
    }

    throw std::runtime_error("Unknown report type");
}

brayns::AbstractSimulationHandlerPtr getHandlerForType(
    const ReportType type,
    const std::string &reportPath,
    const std::string &population,
    const bbp::sonata::Selection &selection)
{
    switch (type)
    {
    case ReportType::BLOODFLOW_PRESSURE:
    case ReportType::BLOODFLOW_SPEED:
    case ReportType::COMPARTMENT:
    case ReportType::SUMMATION:
    case ReportType::SYNAPSE:
        return std::make_shared<SonataReportHandler>(reportPath, population, selection);
    case ReportType::BLOODFLOW_RADII:
        return std::make_shared<VasculatureRadiiHandler>(reportPath, population, selection);
    case ReportType::SPIKES:
        return std::make_shared<SonataSpikeHandler>(reportPath, population, selection);
    default:
        return {nullptr};
    }
}

} // namespace

void PopulationReportManager::loadNodeMapping(
    const SonataNetworkConfig &network,
    const SonataNodePopulationParameters &input,
    const bbp::sonata::Selection &selection,
    std::vector<MorphologyInstance::Ptr> &nodes)
{
    const auto type = input.report_type;
    if (type == ReportType::NONE)
    {
        return;
    }

    const auto &population = input.node_population;
    const auto &simConfig = network.simulationConfig();
    const auto &reportName = input.report_name;
    const auto reportPath = getReportPath(type, simConfig, reportName);
    const auto reportLoader = getLoaderForType(type);
    const auto mapping = reportLoader->loadMapping(reportPath, population, selection);

#pragma omp parallel for
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        const auto &cm = mapping[i];
        nodes[i]->mapSimulation(cm.globalOffset, cm.offsets, cm.compartments);
    }
}

void PopulationReportManager::loadEdgeMapping(
    const SonataNetworkConfig &network,
    const SonataEdgePopulationParameters &input,
    const bbp::sonata::Selection &selection,
    std::vector<SynapseGroup::Ptr> &edges)
{
    const auto &reportName = input.edge_report_name;
    if (reportName.empty())
    {
        return;
    }

    const auto &population = input.edge_population;
    const auto &simConfig = network.simulationConfig();
    const auto reportPath = getReportPath(ReportType::SYNAPSE, simConfig, reportName);
    const auto mapping = EdgeCompartmentLoader().loadMapping(reportPath, population, selection);

#pragma omp parallel for
    for (size_t j = 0; j < edges.size(); ++j)
    {
        edges[j]->mapSimulation(mapping[j].offsets);
    }
}

void PopulationReportManager::addNodeReportHandler(
    const SonataNetworkConfig &network,
    const SonataNodePopulationParameters &input,
    const bbp::sonata::Selection &selection,
    brayns::ModelDescriptorPtr &model)
{
    const auto type = input.report_type;
    if (type == ReportType::NONE)
        return;

    const auto &simConfig = network.simulationConfig();
    const auto &reportName = input.report_name;
    const auto reportPath = getReportPath(type, simConfig, reportName);
    const auto &population = input.node_population;

    auto handler = getHandlerForType(type, reportPath, population, selection);
    if (!handler)
    {
        return;
    }

    model->getModel().setSimulationHandler(handler);
    CircuitExplorerMaterial::setSimulationColorEnabled(model->getModel(), true);
}

void PopulationReportManager::addEdgeReportHandler(
    const SonataNetworkConfig &network,
    const SonataEdgePopulationParameters &input,
    const bbp::sonata::Selection &selection,
    brayns::ModelDescriptorPtr &model)
{
    const auto &reportName = input.edge_report_name;
    if (reportName.empty())
    {
        return;
    }

    const auto &simConfig = network.simulationConfig();
    const auto path = getReportPath(ReportType::SYNAPSE, simConfig, reportName);
    const auto &population = input.edge_population;

    auto handler = getHandlerForType(ReportType::SYNAPSE, path, population, selection);

    model->getModel().setSimulationHandler(handler);
    CircuitExplorerMaterial::setSimulationColorEnabled(model->getModel(), true);
}

void PopulationReportManager::addNodeHandlerToEdges(
    const brayns::ModelDescriptorPtr &nodeModel,
    const std::vector<brayns::ModelDescriptor *> &edgeModels)
{
    const auto &model = nodeModel->getModel();
    if (!model.getSimulationHandler())
        return;

    for (auto edgeModelDescr : edgeModels)
    {
        auto &edgeModel = edgeModelDescr->getModel();
        if (!edgeModel.getSimulationHandler())
        {
            edgeModel.setSimulationHandler(model.getSimulationHandler());
            CircuitExplorerMaterial::setSimulationColorEnabled(edgeModel, true);
        }
    }
}
} // namespace sonataloader
