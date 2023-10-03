/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <brayns/utils/Log.h>
#include <brayns/utils/Timer.h>

#include <io/sonataloader/EdgeLoader.h>
#include <io/sonataloader/LoadContext.h>
#include <io/sonataloader/ModelTypeFinder.h>
#include <io/sonataloader/NodeLoader.h>
#include <io/sonataloader/ParameterCheck.h>
#include <io/sonataloader/Selector.h>
#include <io/util/ProgressUpdater.h>

namespace
{
namespace sl = sonataloader;

class ProgressUpdaterFactory
{
public:
    static ProgressUpdater create(const brayns::LoaderProgress &callback, const SonataLoaderParameters &params)
    {
        auto numSteps = _computeNumSteps(params);
        return ProgressUpdater(callback, numSteps, 0.5f);
    }

private:
    static size_t _computeNumSteps(const SonataLoaderParameters &params)
    {
        size_t count = 0;
        for (auto &node : params.node_population_settings)
        {
            count += node.edge_populations.size() + 1;
        }
        return count;
    }
};

class ConfigReader
{
public:
    static sl::Config read(const std::string &path, const SonataLoaderParameters &parameters)
    {
        auto config = sl::Config(path);
        sl::ParameterCheck::checkInput(config, parameters);
        return config;
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

std::vector<std::shared_ptr<brayns::Model>> SonataLoader::importFromBlob(
    const brayns::Blob &blob,
    const brayns::LoaderProgress &cb,
    const SonataLoaderParameters &params) const
{
    (void)blob;
    (void)cb;
    (void)params;
    throw std::runtime_error("Import from blob not supported");
}

std::vector<std::shared_ptr<brayns::Model>> SonataLoader::importFromFile(
    const std::string &path,
    const brayns::LoaderProgress &callback,
    const SonataLoaderParameters &input) const
{
    const brayns::Timer timer;
    brayns::Log::info("[CE] {}: loading {}.", getName(), path);

    auto config = ConfigReader::read(path, input);

    auto progress = ProgressUpdaterFactory::create(callback, input);
    std::vector<std::shared_ptr<brayns::Model>> result;

    for (auto &nodeParams : input.node_population_settings)
    {
        auto &nodeName = nodeParams.node_population;
        brayns::Log::info("[CE] - Loading {} node population.", nodeName);

        auto nodeMessage = nodeName + " nodes load";
        progress.beginStage(nodeMessage, 2);
        auto nodes = config.getNodes(nodeName);
        auto nodeSelection = sl::NodeSelector::select(config, nodeParams);
        auto nodeModelType = sl::ModelTypeFinder::fromNodes(nodes, config);
        auto nodeModel = std::make_shared<brayns::Model>(nodeModelType);
        auto nodeContext = sl::NodeLoadContext{config, nodeParams, nodes, nodeSelection, *nodeModel, progress};
        sl::NodeLoader::loadNodes(nodeContext);
        result.push_back(std::move(nodeModel));
        progress.endStage();

        for (auto &edgeParams : nodeParams.edge_populations)
        {
            auto &edgeName = edgeParams.edge_population;
            brayns::Log::info("[CE] - Loading {} edge population.", edgeName);

            auto edgeMessage = edgeName + " edges load";
            progress.beginStage(edgeMessage, 2);
            auto edges = config.getEdges(edgeName);
            auto edgeSelection = sl::EdgeSelector::select(config, edgeParams, nodeSelection);
            auto edgeModelType = sl::ModelTypeFinder::fromEdges(edges, edgeParams.load_afferent, config);
            auto edgeModel = std::make_shared<brayns::Model>(edgeModelType);
            auto edgeContext =
                sl::EdgeLoadContext{config, edgeParams, nodes, edges, nodeSelection, edgeSelection, *edgeModel, progress};
            sl::EdgeLoader::loadEdges(edgeContext);
            result.push_back(std::move(edgeModel));
            progress.endStage();
        }
    }

    progress.end("Generating rendering structures. Might take a while");

    auto time = timer.seconds();
    brayns::Log::info("[CE] {}: done in {} second(s).", getName(), time);

    return result;
}
