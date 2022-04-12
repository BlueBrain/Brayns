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

#include <io/sonataloader/EdgeLoader.h>
#include <io/sonataloader/LoadContext.h>
#include <io/sonataloader/NodeLoader.h>
#include <io/sonataloader/ParameterCheck.h>
#include <io/sonataloader/Selector.h>
#include <io/util/ProgressUpdater.h>

namespace
{
struct LoadStageCountComputer
{
    static size_t compute(const SonataLoaderParameters &params)
    {
        size_t count = 0;
        const auto &nodes = params.node_population_settings;
        for (const auto &node : nodes)
        {
            count += 1;

            const auto &edges = node.edge_populations;
            count += edges.size();
        }
        return count;
    }
};
}

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
    const auto network = sonataloader::SonataConfig::readNetwork(circuitConfigPath, simulationConfigPath);
    const auto &circuitConfig = network.circuitConfig();

    sonataloader::ParameterCheck::checkInput(network, input);

    const auto stages = LoadStageCountComputer::compute(input);
    ProgressUpdater progress(callback, stages);

    std::vector<std::unique_ptr<brayns::Model>> result;

    const auto &inputNodes = input.node_population_settings;
    for (const auto &nodeParams : inputNodes)
    {
        result.push_back(std::make_unique<brayns::Model>());
        auto &nodeModel = *(result.back());

        const auto &nodeName = nodeParams.node_population;
        const auto nodes = circuitConfig.getNodePopulation(nodeName);
        const auto nodeSelection = sonataloader::NodeSelector::select(network, nodeParams);
        sonataloader::NodeLoadContext ctxt{network, nodeParams, nodes, nodeSelection, nodeModel, progress};

        brayns::Log::info("[CE] - Loading {} node population.", nodeName);
        progress.beginStage(2);

        sonataloader::NodeLoader::loadNodes(ctxt);

        progress.endStage();

        // Load edges for this node population
        const auto &inputEdges = nodeParams.edge_populations;
        for (const auto &edgeParams : inputEdges)
        {
            result.push_back(std::make_unique<brayns::Model>());
            auto &edgeModel = *(result.back());

            const auto &edgeName = edgeParams.edge_population;
            const auto edges = circuitConfig.getEdgePopulation(edgeName);
            const auto edgeSelection = sonataloader::EdgeSelector::select(network, edgeParams, nodeSelection);
            sonataloader::EdgeLoadContext
                edgeCtxt{network, edgeParams, nodes, edges, nodeSelection, edgeSelection, edgeModel, progress};

            brayns::Log::info("[CE] \t - Loading {} edge population.", edgeName);
            progress.beginStage(2);

            sonataloader::EdgeLoader::loadEdges(edgeCtxt);

            progress.endStage();
        }
    }

    const auto time = timer.seconds();
    brayns::Log::info("[CE] {}: done in {} second(s).", getName(), time);

    return result;
}
