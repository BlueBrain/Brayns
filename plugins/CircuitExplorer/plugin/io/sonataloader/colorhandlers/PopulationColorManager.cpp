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

#include "PopulationColorManager.h"

#include <brayns/common/Log.h>

#include <plugin/io/sonataloader/colorhandlers/edge/CommonEdgeColorHandler.h>
#include <plugin/io/sonataloader/colorhandlers/edge/EndFootColorHandler.h>

#include <plugin/io/sonataloader/colorhandlers/node/SonataNeuronColorHandler.h>
#include <plugin/io/sonataloader/colorhandlers/node/VasculatureColorHandler.h>

#include <plugin/io/sonataloader/data/SonataCells.h>

namespace sonataloader
{
CircuitColorHandler::Ptr PopulationColorManager::createNodeColorHandler(
    const SonataConfig::Data& network, const SonataNodePopulationParameters& lc)
{
    std::string type;
    try
    {
        const auto population =
            network.config.getNodePopulation(lc.node_population);
        type = SonataCells::getPopulationType(population);
    }
    catch (...)
    {
        brayns::Log::warn(
            "[CE] PopulationLoaderManager: Extracting population type "
            "from population properties for {}.",
            lc.node_population);
        type =
            network.config.getNodePopulationProperties(lc.node_population).type;
    }

    if (type == "vasculature")
        return std::make_unique<VasculatureColorHandler>();

    return std::make_unique<SonataNeuronColorHandler>(network.path,
                                                      lc.node_population);
}

CircuitColorHandler::Ptr PopulationColorManager::createEdgeColorHandler(
    const SonataConfig::Data& network, const SonataEdgePopulationParameters& lc)
{
    const auto edgeProperties =
        network.config.getEdgePopulationProperties(lc.edge_population);

    if (edgeProperties.type == "endfoot")
        return std::make_unique<EndFootColorHandler>();

    return std::make_unique<CommonEdgeColorHandler>(network.path,
                                                    lc.edge_population,
                                                    lc.load_afferent);
}
} // namespace sonataloader
