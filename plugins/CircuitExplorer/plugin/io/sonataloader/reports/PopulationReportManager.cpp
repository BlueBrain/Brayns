/* Copyright 2015-2024 Blue Brain Project/EPFL
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

brayns::AbstractSimulationHandlerPtr getHandlerForType(
    const ReportType type, const std::string& reportPath,
    const std::string& population, const bbp::sonata::Selection& selection)
{
    switch (type)
    {
    case ReportType::BLOODFLOW_PRESSURE:
    case ReportType::BLOODFLOW_SPEED:
    case ReportType::COMPARTMENT:
    case ReportType::SUMMATION:
    case ReportType::SYNAPSE:
        return std::make_shared<SonataReportHandler>(reportPath, population,
                                                     selection);
    case ReportType::BLOODFLOW_RADII:
        return std::make_shared<VasculatureRadiiHandler>(reportPath, population,
                                                         selection);
    case ReportType::SPIKES:
        return std::make_shared<SonataSpikeHandler>(reportPath, population,
                                                    selection);
    default:
        return {nullptr};
    }
}

} // namespace

void PopulationReportManager::loadNodeMapping(
    const SonataNodePopulationParameters& input,
    const bbp::sonata::Selection& selection,
    std::vector<MorphologyInstance::Ptr>& nodes)
{
    const auto type = input.report_type;
    if (type == ReportType::NONE)
        return;

    const auto reportLoader = getLoaderForType(type);
    const auto mapping =
        reportLoader->loadMapping(input.report_path, input.node_population,
                                  selection);

#pragma omp parallel for
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        const auto& cm = mapping[i];
        nodes[i]->mapSimulation(cm.globalOffset, cm.offsets, cm.compartments);
    }
}

void PopulationReportManager::loadEdgeMapping(
    const SonataEdgePopulationParameters& input,
    const bbp::sonata::Selection& selection,
    std::vector<SynapseGroup::Ptr>& edges)
{
    if (input.edge_report.empty())
        return;

    // Currently there is only one type of synapse report...
    const auto mapping =
        EdgeCompartmentLoader().loadMapping(input.edge_report,
                                            input.edge_population, selection);

#pragma omp parallel for
    for (size_t j = 0; j < edges.size(); ++j)
        edges[j]->mapSimulation(mapping[j].offsets);
}

void PopulationReportManager::addNodeReportHandler(
    const SonataNodePopulationParameters& input,
    const bbp::sonata::Selection& selection, brayns::ModelDescriptorPtr& model)
{
    const auto type = input.report_type;
    if (type == ReportType::NONE)
        return;

    const auto& path = input.report_path;
    const auto& population = input.node_population;

    auto handler = getHandlerForType(type, path, population, selection);
    if (!handler)
        return;

    model->getModel().setSimulationHandler(handler);
    CircuitExplorerMaterial::setSimulationColorEnabled(model->getModel(), true);
}

void PopulationReportManager::addEdgeReportHandler(
    const SonataEdgePopulationParameters& input,
    const bbp::sonata::Selection& selection, brayns::ModelDescriptorPtr& model)
{
    if (input.edge_report.empty())
        return;

    const auto& path = input.edge_report;
    const auto& population = input.edge_population;

    auto handler =
        getHandlerForType(ReportType::SYNAPSE, path, population, selection);

    model->getModel().setSimulationHandler(handler);
    CircuitExplorerMaterial::setSimulationColorEnabled(model->getModel(), true);
}

void PopulationReportManager::addNodeHandlerToEdges(
    const brayns::ModelDescriptorPtr& nodeModel,
    const std::vector<brayns::ModelDescriptor*>& edgeModels)
{
    const auto& model = nodeModel->getModel();
    if (!model.getSimulationHandler())
        return;

    for (auto edgeModelDescr : edgeModels)
    {
        auto& edgeModel = edgeModelDescr->getModel();
        if (!edgeModel.getSimulationHandler())
        {
            edgeModel.setSimulationHandler(model.getSimulationHandler());
            CircuitExplorerMaterial::setSimulationColorEnabled(edgeModel, true);
        }
    }
}
} // namespace sonataloader
