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

#include "PopulationColorManager.h"

#include <brayns/common/Log.h>

#include <plugin/io/sonataloader/colorhandlers/edge/CommonEdgeColorData.h>
#include <plugin/io/sonataloader/colorhandlers/edge/EndFootColorData.h>

#include <plugin/io/sonataloader/colorhandlers/node/SonataNeuronColorData.h>
#include <plugin/io/sonataloader/colorhandlers/node/VasculatureColorData.h>

#include <plugin/io/sonataloader/data/SonataCells.h>

namespace sonataloader
{
std::unique_ptr<CircuitColorData> PopulationColorManager::createNodeColorHandler(
    const SonataNetworkConfig &network,
    const SonataNodePopulationParameters &lc)
{
    std::string type;
    const auto &config = network.circuitConfig();
    const auto &populationName = lc.node_population;
    try
    {
        const auto population = config.getNodePopulation(populationName);
        type = SonataCells::getPopulationType(population);
    }
    catch (...)
    {
        brayns::Log::warn(
            "[CE] PopulationLoaderManager: Extracting population type "
            "from population properties for {}.",
            lc.node_population);
        type = config.getNodePopulationProperties(populationName).type;
    }

    if (type == "vasculature")
    {
        return std::make_unique<VasculatureColorData>(config, populationName);
    }

    return std::make_unique<SonataNeuronColorData>(config, populationName);
}

std::unique_ptr<CircuitColorData> PopulationColorManager::createEdgeColorHandler(
    const SonataNetworkConfig &network,
    const SonataEdgePopulationParameters &lc)
{
    const auto &config = network.circuitConfig();
    const auto &population = lc.edge_population;
    const auto edgeProperties = config.getEdgePopulationProperties(population);
    const auto loadAfferent = lc.load_afferent;

    if (edgeProperties.type == "endfoot")
    {
        return std::make_unique<EndFootColorData>();
    }

    return std::make_unique<CommonEdgeColorData>(config, population, loadAfferent);
}
} // namespace sonataloader
