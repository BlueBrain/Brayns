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

#include "SonataLoader.h"

#include <brayns/common/Log.h>
#include <brayns/common/Timer.h>
#include <brayns/engine/Scene.h>
#include <brayns/utils/StringUtils.h>

#include <io/sonataloader/LoadContext.h>
#include <io/sonataloader/NodeSelector.h>
#include <io/sonataloader/ParameterCheck.h>
#include <io/sonataloader/colorhandlers/PopulationColorManager.h>
#include <io/sonataloader/populations/PopulationLoadManager.h>
#include <io/sonataloader/reports/PopulationReportManager.h>

using namespace sonataloader;

namespace
{
float computeProgressChunk(const SonataLoaderParameters &loadParameters) noexcept
{
    // Compute how much progress percentage each population load will consume
    const auto &populations = loadParameters.node_population_settings;
    const auto numNodePopulations = populations.size();

    size_t numEdgePopulations = 0u;
    for (const auto &popLoadConfig : populations)
    {
        const auto &edgePopulations = popLoadConfig.edge_populations;
        numEdgePopulations += edgePopulations.size();
    }
    // Each population (wether nodes or edges) have 3 step: geometry, simulation, model creation
    const float chunk = (1.f / static_cast<float>(numNodePopulations * 3 + numEdgePopulations * 3));

    return chunk;
}

void informProgress(const brayns::LoaderProgress &cb, const std::string &msg, float &total, const float chunk)
{
    cb.updateProgress(msg, total);
    total += chunk;
    total = std::max(total, 1.f);
}
} // namespace

std::vector<std::string> SonataLoader::getSupportedExtensions() const
{
    return std::vector<std::string>{".json"};
}

std::string SonataLoader::getName() const
{
    return std::string("SONATA loader");
}

std::vector<std::unique_ptr<brayns::Model>> SonataLoader::importFromBlob(
    brayns::Blob &&blob,
    const brayns::LoaderProgress &cb,
    const SonataLoaderParameters &params) const
{
    (void)blob;
    (void)cb;
    (void)params;
    throw std::runtime_error("Import from blob not supported");
}

std::vector<std::unique_ptr<brayns::Model>> SonataLoader::importFromFile(
    const std::string &path,
    const brayns::LoaderProgress &callback,
    const SonataLoaderParameters &input) const
{
    const brayns::Timer timer;
    brayns::Log::info("[CE] {}: loading {}.", getName(), path);

    // Load config files
    const auto &circuitConfigPath = path;
    const auto &simulationConfigPath = input.simulation_config_path;
    const auto network = SonataConfig::readNetwork(circuitConfigPath, simulationConfigPath);
    const auto &circuitConfig = network.circuitConfig();

    // Check input parameters and data available on disk
    ParameterCheck::checkInput(network, input);

    // Compute progress chunk per population load item
    const float chunk = computeProgressChunk(input);
    float total = 0.f;

    std::vector<std::unique_ptr<brayns::Model>> result;
    for (const auto &nodeSettings : input.node_population_settings)
    {
        const auto &nodeName = nodeSettings.node_population;
        const auto nodePopulation = circuitConfig.getNodePopulation(nodeName);
        const auto nodeSelection = NodeSelector::selectNodes(network, nodeSettings);

        const NodeLoadContext nodeContext{network, nodeSettings, nodePopulation, nodeSelection};

        brayns::Log::info("[CE] Loading {} node population.", nodeName);

        // Select nodes that are going to be loaded

        // Load node data
        const auto nodeIDs = nodeSelection.flatten();
        informProgress(callback, "Loading " + nodeName, total, chunk);
        auto nodes = PopulationLoaderManager::loadNodes(network, nodeSettings, nodeSelection);
        if (nodes.empty())
        {
            brayns::Log::warn("Skipping node population {}, no nodes were loaded", nodeName);
            continue;
        }

        // Load node report mapping, if any
        informProgress(callback, nodeName + ": Loading simulation", total, chunk);
        PopulationReportManager::loadNodeMapping(network, nodeSettings, nodeSelection, nodes);

        // Load edges for this node population
        std::vector<brayns::ModelDescriptor *> edgeModels;
        for (const auto &edge : nodeSettings.edge_populations)
        {
            const auto &edgeName = edge.edge_population;
            brayns::Log::info("[CE] \tLoading {} edge population.", edgeName);

            informProgress(callback, "Loading " + edgeName, total, chunk);
            // Load edge data
            auto edges = PopulationLoaderManager::loadEdges(network, edge, nodeSelection);
            if (edges.empty())
                continue;

            // Map edge geometry to node geometry
            PopulationLoaderManager::mapEdgesToNodes(nodes, edges);

            // Load edge report mapping, if any
            informProgress(callback, edgeName + ": Loading simulation", total, chunk);
            PopulationReportManager::loadEdgeMapping(network, edge, nodeSelection, edges);

            // Add geometry to the edge model
            informProgress(callback, edgeName + ": Generating edge geometry", total, chunk);
            brayns::ModelPtr edgeModel = scene.createModel();
            std::vector<ElementMaterialMap::Ptr> edgeMaterialMaps(nodes.size());
            for (size_t j = 0; j < nodes.size(); ++j)
            {
                edgeMaterialMaps[j] = edges[j]->addToModel(*edgeModel);
                edges[j].reset(nullptr);
            }

            result.push_back(createModelDescriptor(edgeName, path, edgeModel));

            // Create simulation handler, if any
            PopulationReportManager::addEdgeReportHandler(network, edge, nodeSelection, result.back());
            // Add to the synapse model list to set the node simulation handler
            // if this edge model does not have one
            edgeModels.push_back(result.back().get());

            // Create the color handler
            auto edgeColor = PopulationColorManager::createEdgeColorHandler(network, edge);
            _colorManager.registerHandler(result.back(), std::move(edgeColor), nodeIDs, std::move(edgeMaterialMaps));

            brayns::Log::info("[CE] \tLoaded edge population {}.", edgeName);
        }

        informProgress(callback, nodeName + ": Generating node geometry", total, chunk);

        // Add geometry to the node model
        brayns::ModelPtr nodeModel = scene.createModel();
        std::vector<ElementMaterialMap::Ptr> materialMaps(nodes.size());
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            materialMaps[i] = nodes[i]->addToModel(*nodeModel);
            nodes[i].reset(nullptr);
        }

        // Create the model descriptor
        result.push_back(createModelDescriptor(nodeName, path, nodeModel));

        // Create simulation handler
        PopulationReportManager::addNodeReportHandler(network, nodeSettings, nodeSelection, result.back());
        // Update all the edge populations that does not have a simulation of
        // their own
        PopulationReportManager::addNodeHandlerToEdges(result.back(), edgeModels);

        // Create the color handler
        auto nodeColor = PopulationColorManager::createNodeColorHandler(network, nodeSettings);
        _colorManager.registerHandler(result.back(), std::move(nodeColor), nodeIDs, std::move(materialMaps));

        brayns::Log::info("[CE] Loaded node population {}.", nodeName);
    }

    TransferFunctionUtils::set(scene.getTransferFunction());

    const auto time = timer.elapsed();
    brayns::Log::info("[CE] {}: done in {} second(s).", getName(), time);

    return result;
}
