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

#pragma once

#include <bbp/sonata/edges.h>

#include <brayns/common/MathTypes.h>

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
     * @brief return the list of source node ids for the given selection of
     * edges in the given edge population
     */
    static std::vector<uint64_t> getSourceNodes(const Edges& population,
                                                const Selection& edgeSelection);

    /**
     * @brief return the list of target node ids for the given selection of
     * edges in the given edge population
     */
    static std::vector<uint64_t> getTargetNodes(const Edges& population,
                                                const Selection& edgeSelection);

    /**
     * @brief return the list of afferent section ids (afferent_section_id
     * dataset) for the given edge selection in the given edge population
     */
    static std::vector<int32_t> getAfferentSectionIds(
        const Edges& population, const Selection& edgeSelection);

    /**
     * @brief return the list of efferent section ids (efferent_section_id
     * dataset) for the given edge selection in the given edge population
     */
    static std::vector<int32_t> getEfferentSectionIds(
        const Edges& population, const Selection& edgeSelection);

    /**
     * @brief return the list of afferent surface 3D positions
     * (afferent_surface_x, afferent_surface_y, afferent_surface_z dataset
     * combined) for the given edge selection in the given edge population
     */
    static std::vector<brayns::Vector3f> getAfferentSurfacePos(
        const Edges& population, const Selection& selection);

    /**
     * @brief return the list of efferent surface 3D positions
     * (efferent_surface_x, efferent_surface_y, efferent_surface_z dataset
     * combined) for the given edge selection in the given edge population
     */
    static std::vector<brayns::Vector3f> getEfferentSurfacePos(
        const Edges& population, const Selection& selection);

    /**
     * @brief return the list of afferent section distances
     * (afferent_section_pos dataset) for the given edge selection in the given
     * edge population. The afferent section distances are a normalized paramter
     * between 0.0 and 1.0 that denotes the relative distance from the section
     * start and section length where an edge is positioned.
     */
    static std::vector<float> getAfferentSectionDistances(
        const Edges& population, const Selection& selection);

    /**
     * @brief return the list of efferent section distances
     * (efferent_section_pos dataset) for the given edge selection in the given
     * edge population. The afferent section distances are a normalized paramter
     * between 0.0 and 1.0 that denotes the relative distance from the section
     * start and section length where an edge is positioned.
     */
    static std::vector<float> getEfferentSectionDistances(
        const Edges& population, const Selection& selection);

    /**
     * @brief return the list of efferent section ids (efferent_section_id
     * dataset) for the given edge selection in the given edge synapse_astroctye
     * population (not enforced)
     */
    static std::vector<int32_t> getEfferentAstrocyteSectionIds(
        const Edges& population, const Selection& selection);

    /**
     * @brief return the list of efferent section distances
     * (efferent_section_pos dataset) for the given edge selection in the given
     * edge synapse_astroctye population (not enforced)
     */
    static std::vector<float> getEfferentAstrocyteSectionDistances(
        const Edges& population, const Selection& selection);

    /**
     * @brief return the list of efferent endfeet surface 3D positions for the
     *        given edge selection in the given edge endfeet population (not
     * enforced)
     */
    static std::vector<brayns::Vector3f> getEndFeetSurfacePos(
        const Edges& population, const Selection& selection);

    /**
     * @brief return the list of endfeet edge ids for the given edge selection
     *        in the given edge endfeet population (not enforced)
     */
    static std::vector<uint64_t> getEndFeetIds(const Edges& population,
                                               const Selection& selection);
};
} // namespace sonataloader
