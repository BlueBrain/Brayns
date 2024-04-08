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

#include <bbp/sonata/edges.h>

#include <brayns/core/utils/MathTypes.h>

namespace sonataloader
{
/**
 * @brief Wrapper around bbp::sonata::EdgePopulation for chemical/electrical/glialglial/synapse_astrocyte edges.
 */
class Synapses
{
private:
    using Edges = bbp::sonata::EdgePopulation;
    using Selection = bbp::sonata::Selection;

public:
    /**
     * @brief Get the source nodes for the given selection of edges.
     * @param edges Edge population.
     * @param selection Selection of edges.
     * @return std::vector<uint64_t>
     */
    static std::vector<uint64_t> getSourceNodes(const Edges &edges, const Selection &selection);

    /**
     * @brief Get the target nodes for the given selection of edges.
     * @param edges Edge population.
     * @param selection Selection of edges.
     * @return std::vector<uint64_t>
     */
    static std::vector<uint64_t> getTargetNodes(const Edges &edges, const Selection &selection);

    /**
     * @brief Get the afferent surface position for the given selection of edges.
     * @param edges Edge population.
     * @param selection Selection of edges.
     * @return std::vector<uint64_t>
     * @throws std::runtime_error if the population misses the needed attributes.
     */
    static std::vector<brayns::Vector3f> getAfferentSurfacePos(const Edges &edges, const Selection &selection);

    /**
     * @brief Get the efferent surface position for the given selection of edges.
     * @param edges Edge population.
     * @param selection Selection of edges.
     * @return std::vector<uint64_t>
     * @throws std::runtime_error if the population misses the needed attributes.
     */
    static std::vector<brayns::Vector3f> getEfferentSurfacePos(const Edges &population, const Selection &selection);

    /**
     * @brief Get the efferent astrocyte position for the given selection of edges.
     * @param edges Edge population.
     * @param selection Selection of edges.
     * @return std::vector<uint64_t>
     * @throws std::runtime_error if the population misses the needed attributes.
     */
    static std::vector<brayns::Vector3f> getEfferentAstrocyteCenterPos(const Edges &edges, const Selection &selection);

    /**
     * @brief Returns the endfeet ids from endfeet populations from the given edge selection.
     * @param population Edge population.
     * @param selection Selection of edges.
     * @return std::vector<uint64_t>
     * @throws std::runtime_error if the population misses the needed attributes.
     */
    static std::vector<uint64_t> getEndFeetIds(const Edges &edges, const Selection &selection);
};
} // namespace sonataloader
