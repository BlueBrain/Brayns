/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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
#include <plugin/api/VasculatureRadiiSimulation.h>

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
class NodeReportLoaderTable
{
public:
    NodeReportLoaderTable()
    {
        registerReportLoader<ReportType::BLOODFLOW_PRESSURE,
                             NodeVasculatureReportLoader>();
        registerReportLoader<ReportType::BLOODFLOW_RADII,
                             NodeVasculatureReportLoader>();
        registerReportLoader<ReportType::BLOODFLOW_SPEED,
                             NodeVasculatureReportLoader>();
        registerReportLoader<ReportType::COMPARTMENT, NodeCompartmentLoader>();
        registerReportLoader<ReportType::SPIKES, NodeSpikeLoader>();
        registerReportLoader<ReportType::SUMMATION, NodeCompartmentLoader>();
    }

    template <ReportType type, typename T,
              typename =
                  std::enable_if_t<std::is_base_of<NodeReportLoader, T>::value>>
    void registerReportLoader()
    {
        _table[type] = std::make_unique<T>();
    }

    const NodeReportLoader& getLoader(const ReportType& type) const
    {
        auto it = _table.find(type);
        if (it == _table.end())
        {
            const auto typeStr = brayns::enumToString<ReportType>(type);
            throw std::runtime_error(
                "SonataLoader: Cannot find report loader for " + typeStr);
        }

        return *(it->second);
    }

private:
    std::unordered_map<ReportType, NodeReportLoader::Ptr> _table;
};
} // namespace

void PopulationReportManager::loadNodeMapping(
    const SonataNodePopulationParameters& input,
    const bbp::sonata::Selection& selection,
    std::vector<MorphologyInstance::Ptr>& nodes)
{
    static const NodeReportLoaderTable NODEREPORT_TABLE;

    const auto type = input.report_type;
    if (type == ReportType::NONE)
        return;

    const auto& reportLoader = NODEREPORT_TABLE.getLoader(type);
    const auto mapping =
        reportLoader.loadMapping(input.report_path, input.node_population,
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
    if (input.report_type == ReportType::NONE)
        return;

    const auto& path = input.report_path;
    const auto& population = input.node_population;

    brayns::AbstractSimulationHandlerPtr handler{nullptr};
    if (input.report_type == ReportType::SPIKES)
        handler =
            std::make_shared<SonataSpikeHandler>(path, population, selection);
    else if (input.report_type == ReportType::BLOODFLOW_RADII)
        handler = std::make_shared<VasculatureRadiiHandler>(path, population,
                                                            selection);
    else
        handler =
            std::make_shared<SonataReportHandler>(path, population, selection);

    model->getModel().setSimulationHandler(handler);
    CircuitExplorerMaterial::setSimulationColorEnabled(model->getModel(), true);

    if (input.report_type == ReportType::BLOODFLOW_RADII)
    {
        VasculatureRadiiSimulation::registerModel(model);
        model->addOnRemoved([](const brayns::ModelDescriptor& model) {
            VasculatureRadiiSimulation::unregisterModel(model.getModelID());
        });
    }
}

void PopulationReportManager::addEdgeReportHandler(
    const SonataEdgePopulationParameters& input,
    const bbp::sonata::Selection& selection, brayns::ModelDescriptorPtr& model)
{
    if (input.edge_report.empty())
        return;

    auto handler =
        std::make_shared<SonataReportHandler>(input.edge_report,
                                              input.edge_population, selection);
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
