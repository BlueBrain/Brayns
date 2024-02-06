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

#include <bbp/sonata/nodes.h>

#include <brayns/utils/MathTypes.h>

namespace sonataloader
{
/**
 * @brief Wrapper around bbp::sonata::NodePopulation for biophysical/astrocyte/point neuron.
 */
class Cells
{
public:
    using Nodes = bbp::sonata::NodePopulation;
    using Selection = bbp::sonata::Selection;

    /**
     * @brief Returns the population type.
     * @return std::string
     * @throws std::runtime_error if the population lacks the attribute.
     */
    static std::string getPopulationType(const Nodes &nodes);

    /**
     * @brief Returns the list of morphology names.
     * @return std::vector<std::string>
     * @throws std::runtime_error if the population lacks the attribute.
     */
    static std::vector<std::string> getMorphologies(const Nodes &nodes, const Selection &selection);

    /**
     * @brief Returns the position of every cell.
     * @return std::vector<brayns::Vector3f>
     * @throws std::runtime_error if the population lacks the attribute.
     */
    static std::vector<brayns::Vector3f> getPositions(const Nodes &nodes, const Selection &selection);

    /**
     * @brief Returns the rotation of every cell.
     * @return std::vector<brayns::Vector3f>
     * @throws std::runtime_error if the population lacks the attribute.
     */
    static std::vector<brayns::Quaternion> getRotations(const Nodes &nodes, const Selection &selection);

    /**
     * @brief Returns the layer of every cell.
     * @return std::vector<std::string>
     * @throws std::runtime_error if the population lacks the attribute.
     */
    static std::vector<std::string> getLayers(const Nodes &nodes, const Selection &selection);

    /**
     * @brief Returns the region of every cell.
     * @return std::vector<std::string>
     * @throws std::runtime_error if the population lacks the attribute.
     */
    static std::vector<std::string> getRegions(const Nodes &nodes, const Selection &selection);

    /**
     * @brief Returns the m-type of every cell.
     * @return std::vector<std::string>
     * @throws std::runtime_error if the population lacks the attribute.
     */
    static std::vector<std::string> getMTypes(const Nodes &nodes, const Selection &selection);

    /**
     * @brief Returns the e-type of every cell.
     * @return std::vector<std::string>
     * @throws std::runtime_error if the population lacks the attribute.
     */
    static std::vector<std::string> getETypes(const Nodes &nodes, const Selection &selection);
};
} // namespace sonataloader
