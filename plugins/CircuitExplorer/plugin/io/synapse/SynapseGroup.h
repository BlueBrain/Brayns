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

#include <brayns/engine/Model.h>

#include <plugin/api/CircuitColorHandler.h>
#include <plugin/io/morphology/MorphologyInstance.h>
#include <plugin/io/synapse/SynapseMaterialMap.h>

#include <unordered_map>

/**
 * @brief The SynapseGroup class is the base class to implement representations
 * of a cell edges (synapses) geometry, and provides functionality to add
 *        simulation mapping based on the cell data.
 */
class SynapseGroup
{
public:
    using Ptr = std::unique_ptr<SynapseGroup>;

    virtual ~SynapseGroup() = default;

    /**
     * @brief mapToCell adjust this SynapseGroup geometry to the correct
     * position based on the morphology geometry given as parameter
     */
    virtual void mapToCell(const MorphologyInstance&) = 0;

    /**
     * @brief mapSimulation sets the edge geometries simulation offset, which is
     *        given as a map of edge id -> simulation array index
     */
    virtual void mapSimulation(
        const std::unordered_map<uint64_t, uint64_t>&) = 0;

    /**
     * @brief addToModel adds all the edge geometries on this SynapseGroup to
     * the model given as parameter
     */
    virtual SynapseMaterialMap::Ptr addToModel(brayns::Model& model) const = 0;
};
