/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "PopulationType.h"

#include "Cells.h"

namespace sonataloader
{
std::string PopulationType::getNodeType(const std::string &name, const Config &config)
{
    auto population = config.getNodes(name);
    return getNodeType(population, config);
}

std::string PopulationType::getNodeType(const bbp::sonata::NodePopulation &nodes, const Config &config)
{
    try
    {
        return Cells::getPopulationType(nodes);
    }
    catch (...)
    {
    }

    auto properties = config.getNodesProperties(nodes.name());
    return properties.type;
}

std::string PopulationType::getEdgeType(const std::string &name, const Config &config)
{
    auto properties = config.getEdgesProperties(name);
    return properties.type;
}

std::string PopulationType::getEdgeType(const bbp::sonata::EdgePopulation &edges, const Config &config)
{
    return getEdgeType(edges.name(), config);
}
}
