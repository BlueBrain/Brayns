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

#include "SonataLoader.h"

#include <brayns/common/Log.h>
#include <brayns/common/Timer.h>
#include <brayns/engine/Scene.h>
#include <brayns/utils/StringUtils.h>

#include <plugin/api/CircuitColorManager.h>
#include <plugin/api/MaterialUtils.h>

#include <plugin/io/sonataloader/ParameterCheck.h>
#include <plugin/io/sonataloader/colorhandlers/PopulationColorManager.h>
#include <plugin/io/sonataloader/data/SonataSelection.h>
#include <plugin/io/sonataloader/populations/PopulationLoadManager.h>
#include <plugin/io/sonataloader/reports/PopulationReportManager.h>

#include <plugin/io/util/TransferFunctionUtils.h>

using namespace sonataloader;

namespace
{
auto selectNodes(const bbp::sonata::CircuitConfig& config,
                 const SonataNodePopulationParameters& lc)
{
    NodeSelection selection;
    selection.select(config, lc.node_population, lc.node_sets);
    selection.select(lc.node_ids);
    selection.select(lc.report_type, lc.report_path, lc.node_population);
    const auto selected = selection.intersection(lc.node_percentage);
    if (selected.empty())
        throw std::runtime_error("SonataLoader: Empty node selection for " +
                                 lc.node_population);
    return selected;
}

brayns::ModelDescriptorPtr createModelDescriptor(const std::string& name,
                                                 const std::string& path,
                                                 brayns::ModelPtr& model)
{
    model->updateBounds();
    brayns::Transformation transform;
    transform.setRotationCenter(model->getBounds().getCenter());
    auto modelDescriptor =
        std::make_shared<brayns::ModelDescriptor>(std::move(model), name, path,
                                                  brayns::ModelMetadata());
    modelDescriptor->setName(name);
    modelDescriptor->setTransformation(transform);
    return modelDescriptor;
}

void informProgress(const brayns::LoaderProgress& cb, const std::string& msg,
                    float& total, const float chunk)
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

std::vector<brayns::ModelDescriptorPtr> SonataLoader::importFromBlob(
    brayns::Blob&&, const brayns::LoaderProgress&,
    const SonataLoaderParameters&, brayns::Scene&) const
{
    throw std::runtime_error("Sonata loader: import from blob not supported");
}

std::vector<brayns::ModelDescriptorPtr> SonataLoader::importFromFile(
    const std::string& path, const brayns::LoaderProgress& callback,
    const SonataLoaderParameters& input, brayns::Scene& scene) const
{
    const brayns::Timer timer;
    brayns::Log::info("[CE] {}: loading {}.", getName(), path);

    // Parse and check config and load parameters
    const SonataConfig::Data network = SonataConfig::readNetwork(path);
    ParameterCheck::checkInput(network.config, input);

    // Compute how much progress percentage each population load will consume
    const auto numNodes = input.node_population_settings.size();
    size_t numEdges = 0u;
    for (const auto& popLoadConfig : input.node_population_settings)
        numEdges += popLoadConfig.edge_populations.size();
    float total = 0.f;
    const float chunk = (1.f / static_cast<float>(numNodes * 3 + numEdges * 3));

    std::vector<brayns::ModelDescriptorPtr> result;
    for (const auto& nodeSettings : input.node_population_settings)
    {
        const auto& nodeName = nodeSettings.node_population;

        brayns::Log::info("[CE] Loading {} node population.", nodeName);

        // Select nodes that are going to be loaded
        const auto nodeSelection = selectNodes(network.config, nodeSettings);

        // Load node data
        const auto nodeIDs = nodeSelection.flatten();
        informProgress(callback, "Loading " + nodeName, total, chunk);
        auto nodes = PopulationLoaderManager::loadNodes(network, nodeSettings,
                                                        nodeSelection);
        if (nodes.empty())
            continue;

        // Load node report mapping, if any
        informProgress(callback, nodeName + ": Loading simulation", total,
                       chunk);
        PopulationReportManager::loadNodeMapping(nodeSettings, nodeSelection,
                                                 nodes);

        // Load edges for this node population
        std::vector<brayns::ModelDescriptor*> edgeModels;
        for (const auto& edge : nodeSettings.edge_populations)
        {
            const auto& edgeName = edge.edge_population;
            brayns::Log::info("[CE] \tLoading {} edge population.", edgeName);

            informProgress(callback, "Loading " + edgeName, total, chunk);
            // Load edge data
            auto edges = PopulationLoaderManager::loadEdges(network, edge,
                                                            nodeSelection);
            if (edges.empty())
                continue;

            // Map edge geometry to node geometry
            PopulationLoaderManager::mapEdgesToNodes(nodes, edges);

            // Load edge report mapping, if any
            informProgress(callback, edgeName + ": Loading simulation", total,
                           chunk);
            PopulationReportManager::loadEdgeMapping(edge, nodeSelection,
                                                     edges);

            // Add geometry to the edge model
            informProgress(callback, edgeName + ": Generating edge geometry",
                           total, chunk);
            brayns::ModelPtr edgeModel = scene.createModel();
            std::vector<ElementMaterialMap::Ptr> edgeMaterialMaps(nodes.size());
            for (size_t j = 0; j < nodes.size(); ++j)
            {
                edgeMaterialMaps[j] = edges[j]->addToModel(*edgeModel);
                edges[j].reset(nullptr);
            }

            result.push_back(createModelDescriptor(edgeName, path, edgeModel));

            // Create simulation handler, if any
            PopulationReportManager::addEdgeReportHandler(edge, nodeSelection,
                                                          result.back());
            // Add to the synapse model list to set the node simulation handler
            // if this edge model does not have one
            edgeModels.push_back(result.back().get());

            // Create the color handler
            auto edgeColor =
                PopulationColorManager::createEdgeColorHandler(network, edge);
            _colorManager.registerHandler(result.back(), std::move(edgeColor),
                                          nodeIDs, std::move(edgeMaterialMaps));

            brayns::Log::info("[CE] \tLoaded edge population {}.", edgeName);
        }

        informProgress(callback, nodeName + ": Generating node geometry", total,
                       chunk);

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
        PopulationReportManager::addNodeReportHandler(nodeSettings,
                                                      nodeSelection,
                                                      result.back());
        // Update all the edge populations that does not have a simulation of
        // their own
        PopulationReportManager::addNodeHandlerToEdges(result.back(),
                                                       edgeModels);

        // Create the color handler
        auto nodeColor =
            PopulationColorManager::createNodeColorHandler(network,
                                                           nodeSettings);
        _colorManager.registerHandler(result.back(), std::move(nodeColor),
                                      nodeIDs, std::move(materialMaps));

        brayns::Log::info("[CE] Loaded node population {}.", nodeName);
    }

    TransferFunctionUtils::set(scene.getTransferFunction());

    const auto time = timer.elapsed();
    brayns::Log::info("[CE] {}: done in {} second(s).", getName(), time);

    return result;
}
