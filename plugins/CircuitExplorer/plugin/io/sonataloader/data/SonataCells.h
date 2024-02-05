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

#include <bbp/sonata/nodes.h>

#include <brayns/common/MathTypes.h>

namespace sonataloader
{
/**
 * @brief The SonataCells class is in charge of quering cell information from
 * the node network files
 */
class SonataCells
{
public:
    using Nodes = bbp::sonata::NodePopulation;
    using Selection = bbp::sonata::Selection;

    /**
     * @brief returns a string representing the node population type
     * (biophysical, astrocyte, vasculature, ...)
     */
    static std::string getPopulationType(const Nodes& nodes);

    /**
     * @brief returns the list of morphology names from the given node
     * population and for the given node selection
     */
    static std::vector<std::string> getMorphologies(const Nodes& nodes,
                                                    const Selection& selection);

    /**
     * @brief returns the list of 3D position from the given node population
     *        and for the given node selection
     */
    static std::vector<brayns::Vector3f> getPositions(
        const Nodes& nodes, const Selection& selection);

    /**
     * @brief returns the list of Quaternion rotations from the given node
     * population and for the given node selection
     */
    static std::vector<brayns::Quaternion> getRotations(
        const Nodes& nodes, const Selection& selection);

    /**
     * @brief returns the list of neuronal layers from the given node population
     *        and for the given node selection
     */
    static std::vector<std::string> getLayers(const Nodes& population,
                                              const Selection& selection);

    /**
     * @brief returns the list of neuronal regions from the given node
     * population and for the given node selection
     */
    static std::vector<std::string> getRegions(const Nodes& population,
                                               const Selection& selection);

    /**
     * @brief returns the list of morphological types from the given node
     * population and for the given node selection
     */
    static std::vector<std::string> getMTypes(const Nodes& population,
                                              const Selection& selection);

    /**
     * @brief returns the list of electro-physiological types from the given
     * node population and for the given node selection
     */
    static std::vector<std::string> getETypes(const Nodes& population,
                                              const Selection& selection);
};
} // namespace sonataloader
