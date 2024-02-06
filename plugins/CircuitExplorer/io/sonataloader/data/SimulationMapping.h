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

#include <bbp/sonata/report_reader.h>

namespace sonataloader
{
/**
 * @brief The SonataSimulationMapping class provides optimized access to SONATA
 * simulation node list / mapping during the loading process
 */
class SimulationMapping
{
public:
    /**
     * @brief return the list of node IDs affected by a given population of a
     * given report
     */
    static std::vector<bbp::sonata::NodeID> getCompartmentNodes(
        const std::string &reportPath,
        const std::string &population);

    /**
     * @brief return the mapping for each reported element of the given list of
     * node ids in the given population of the given report. The mapping is
     * given as a vector of pairs <node id, element id>, with node id being the
     * id of the node affected, and element id being the id of the section of
     * the node affected. The same pair <node, element> may appear more than
     * once, denoting that the given element of the given node has multiple
     * compartments reported.
     */
    static std::vector<bbp::sonata::CompartmentID> getCompartmentMapping(
        const std::string &reportPath,
        const std::string &population,
        const std::vector<bbp::sonata::NodeID> &nodeIds);
};
} // namespace sonataloader
