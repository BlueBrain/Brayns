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

#pragma once

#include <bbp/sonata/edges.h>

#include <brayns/utils/MathTypes.h>

namespace sonataloader
{
/**
 * @brief The SonataSynapses class is in charge of quering synapse (edge)
 * information from the edge network files
 */
class SonataSynapses
{
private:
    using Edges = bbp::sonata::EdgePopulation;
    using Selection = bbp::sonata::Selection;

public:
    /**
     * @brief Returns the "model_type" population entry value
     *
     * @param population
     * @return std::string
     */
    static std::string getPopulationType(const Edges &population);

    /**
     * @brief Return the source node ids of the given edge selection
     *
     * @param population
     * @param edgeSelection
     * @return std::vector<uint64_t>
     */
    static std::vector<uint64_t> getSourceNodes(const Edges &population, const Selection &edgeSelection);

    /**
     * @brief Return the target node ids of the given edge selection
     *
     * @param population
     * @param edgeSelection
     * @return std::vector<uint64_t>
     */
    static std::vector<uint64_t> getTargetNodes(const Edges &population, const Selection &edgeSelection);

    /**
     * @brief Return the afferent surface position for the given edge selection
     *
     * @param population
     * @param selection
     * @return std::vector<brayns::Vector3f>
     */
    static std::vector<brayns::Vector3f> getAfferentSurfacePos(const Edges &population, const Selection &selection);

    /**
     * @brief Return the afferent surface position for the given edge selection
     *
     * @param population
     * @param selection
     * @return std::vector<brayns::Vector3f>
     */
    static std::vector<brayns::Vector3f> getEfferentSurfacePos(const Edges &population, const Selection &selection);

    /**
     * @brief Return the efferent astrocyte center position from synapse_astroctye populations for the given edge
     * selection
     *
     * @param population
     * @param selection
     * @return std::vector<brayns::Vector3f>
     */
    static std::vector<brayns::Vector3f> getEfferentAstrocyteCenterPos(
        const Edges &population,
        const Selection &selection);

    /**
     * @brief Return the endfeet ids from endfeet populations fro the given edge selection
     *
     * @param population
     * @param selection
     * @return std::vector<uint64_t>
     */
    static std::vector<uint64_t> getEndFeetIds(const Edges &population, const Selection &selection);
};
} // namespace sonataloader
