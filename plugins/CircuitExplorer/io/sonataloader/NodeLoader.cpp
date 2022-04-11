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

#include "NodeLoader.h"

#include <io/sonataloader/LoaderTable.h>
#include <io/sonataloader/data/SonataCells.h>

namespace
{
namespace sl = sonataloader;

struct PopulationTypeResolver
{
    static std::string resolve(sl::NodeLoadContext &ctxt)
    {
        const auto &population = ctxt.population;
        const auto populationName = population.name();
        try
        {
            return sl::SonataCells::getPopulationType(population);
        }
        catch (...)
        {
            const auto &network = ctxt.config;
            const auto &config = network.circuitConfig();
            const auto populationProperties = config.getNodePopulationProperties(populationName);
            return populationProperties.type;
        }

        throw std::runtime_error("Could not determine the population type of node population " + populationName);
    }
};
}

namespace sonataloader
{
void NodeLoader::loadNodes(NodeLoadContext &ctxt)
{
    const auto loaderTable = NodeLoaderTable::create();
    const auto populationType = PopulationTypeResolver::resolve(ctxt);
    const auto &loader = loaderTable.getLoader(populationType);
    loader.load(ctxt);
}
}