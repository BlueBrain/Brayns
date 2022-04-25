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

#include <api/coloring/IColorData.h>

#include <bbp/sonata/nodes.h>

namespace sonataloader
{
/**
 * @brief Provides color data for edge populations chemical, glialglial, electrical_synapse, synapse_astrocyte
 * and endfeet populations. The data comes from the node population they are connected to (For example, pre synaptic
 * neuron population on an afferent-loaded chemical edge population)
 */
class CommonEdgeColorData final : public IColorData
{
public:
    /**
     * @brief Construct a new Common Edge Color Data object
     *
     * @param connectedNodes
     */
    CommonEdgeColorData(bbp::sonata::NodePopulation connectedNodes);

    /**
     * @brief getMethods
     * @return
     */
    std::vector<std::string> getMethods() const noexcept override;

    /**
     * @brief getMethodVariables
     * @param method
     * @return
     */
    std::vector<std::string> getMethodVariables(const std::string &method) const override;

    /**
     * @brief getMethodValuesForIDs
     * @param method
     * @param ids
     * @return
     */
    std::vector<std::string> getMethodValuesForIDs(const std::string &method, const std::vector<uint64_t> &ids)
        const override;

protected:
    const bbp::sonata::NodePopulation _nodes;
};
} // namespace sonataloader
