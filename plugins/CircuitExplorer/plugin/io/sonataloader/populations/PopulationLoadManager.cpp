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

#include "PopulationLoadManager.h"

#include <brayns/common/Log.h>

#include <plugin/io/sonataloader/data/SonataCells.h>

#include <plugin/io/sonataloader/populations/edges/ChemicalSynapsePopulationLoader.h>
#include <plugin/io/sonataloader/populations/edges/ElectricalSynapsePopulationLoader.h>
#include <plugin/io/sonataloader/populations/edges/EndFootPopulationLoader.h>
#include <plugin/io/sonataloader/populations/edges/GlialGlialPopulationLoader.h>
#include <plugin/io/sonataloader/populations/edges/SynapseAstrocytePopulationLoader.h>

#include <plugin/io/sonataloader/populations/nodes/AstrocytePopulationLoader.h>
#include <plugin/io/sonataloader/populations/nodes/BiophysicalPopulationLoader.h>
#include <plugin/io/sonataloader/populations/nodes/PointNeuronPopulationLoader.h>
#include <plugin/io/sonataloader/populations/nodes/VasculaturePopulationLoader.h>

#include <unordered_map>

namespace sonataloader
{
namespace
{
template <typename Super>
class PopulationLoaderTable
{
public:
    template <typename T,
              typename = std::enable_if_t<std::is_base_of<Super, T>::value>>
    void registerLoader()
    {
        auto loader = std::make_unique<T>();
        const auto name = loader->getType();
        _nodes[name] = std::move(loader);
    }

    Super* getLoader(const std::string& name)
    {
        auto it = _nodes.find(name);
        if (it != _nodes.end())
            return it->second.get();

        return nullptr;
    }

private:
    std::unordered_map<std::string, typename Super::Ptr> _nodes;
};

class NodePopulationLoaders
{
public:
    NodePopulationLoaders()
    {
        _table.registerLoader<AstrocytePopulationLoader>();
        _table.registerLoader<BiophysicalPopulationLoader>();
        _table.registerLoader<PointNeuronPopulationLoader>();
        _table.registerLoader<VasculaturePopulationLoader>();
    }

    const NodePopulationLoader& getNodeLoader(const std::string& name)
    {
        auto nlptr = _table.getLoader(name);
        if (!nlptr)
            throw std::invalid_argument("No node population loader for type " +
                                        name);

        return *nlptr;
    }

private:
    PopulationLoaderTable<NodePopulationLoader> _table;
};

class EdgePopulationLoaders
{
public:
    EdgePopulationLoaders()
    {
        _table.registerLoader<ChemicalSynapsePopulationLoader>();
        _table.registerLoader<ElectricalSynapsePopulationLoader>();
        _table.registerLoader<EndFootPopulationLoader>();
        _table.registerLoader<GlialGlialPopulationLoader>();
        _table.registerLoader<SynapseAstrocytePopulationLoader>();
    }

    const EdgePopulationLoader& getEdgeLoader(const std::string& name)
    {
        auto elptr = _table.getLoader(name);
        if (!elptr)
            throw std::invalid_argument("No edge population loader for type " +
                                        name);

        return *elptr;
    }

private:
    PopulationLoaderTable<EdgePopulationLoader> _table;
};
} // namespace

std::vector<SynapseGroup::Ptr> PopulationLoaderManager::loadEdges(
    const SonataConfig::Data& networkConfig,
    const SonataEdgePopulationParameters& lc,
    const bbp::sonata::Selection& nodeSelection)
{
    const auto edgeType =
        networkConfig.config.getEdgePopulationProperties(lc.edge_population)
            .type;
    EdgePopulationLoaders epl;
    const auto& loader = epl.getEdgeLoader(edgeType);

    return loader.load(networkConfig, lc, nodeSelection);
}

std::vector<MorphologyInstance::Ptr> PopulationLoaderManager::loadNodes(
    const SonataConfig::Data& networkData,
    const SonataNodePopulationParameters& lc,
    const bbp::sonata::Selection& nodeSelection)
{
    std::string nodeType;
    {
        try
        {
            const auto nodes =
                networkData.config.getNodePopulation(lc.node_population);
            nodeType = SonataCells::getPopulationType(nodes);
        }
        catch (...)
        {
            brayns::Log::warn(
                "[CE] PopulationLoaderManager: Extracting population "
                "type from population properties for {}.",
                lc.node_population);
            auto nodeProperties =
                networkData.config.getNodePopulationProperties(
                    lc.node_population);
            nodeType = std::move(nodeProperties.type);
        }
    }
    NodePopulationLoaders npl;
    const auto& loader = npl.getNodeLoader(nodeType);

    return loader.load(networkData, lc, nodeSelection);
}

void PopulationLoaderManager::mapEdgesToNodes(
    const std::vector<MorphologyInstance::Ptr>& nodes,
    std::vector<SynapseGroup::Ptr>& edges)
{
#pragma omp parallel for
    for (size_t j = 0; j < nodes.size(); ++j)
        edges[j]->mapToCell(*nodes[j]);
}
} // namespace sonataloader
