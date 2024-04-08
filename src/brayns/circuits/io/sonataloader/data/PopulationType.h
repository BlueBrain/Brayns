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

#pragma once

#include "Config.h"

#include <bbp/sonata/edges.h>
#include <bbp/sonata/nodes.h>

namespace sonataloader
{
/**
 * @brief Finds the type of the sonata populations
 */
class PopulationType
{
public:
    /**
     * @brief Returns the type of the node population.
     * @param name Name of the node population.
     * @param config Sonata network configuration.
     * @return std::string Nodes type.
     */
    static std::string getNodeType(const std::string &name, const Config &config);
    static std::string getNodeType(const bbp::sonata::NodePopulation &nodes, const Config &config);

    /**
     * @brief Returns the type of the edge population.
     * @param name Name of the edge population.
     * @param config Sonata network configuration.
     * @return std::string Edge type.
     */
    static std::string getEdgeType(const std::string &name, const Config &config);
    static std::string getEdgeType(const bbp::sonata::EdgePopulation &edges, const Config &config);
};
}
