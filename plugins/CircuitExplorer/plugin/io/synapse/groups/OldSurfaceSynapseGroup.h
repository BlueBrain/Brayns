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

#include <plugin/io/synapse/SynapseGroup.h>

#include <brayns/common/geometry/SDFGeometry.h>

/**
 * @brief The EndFootGroup class implements a SynapseGroup funcionality for
 * synapeses loaded from BlueConfig/CircuitConfig (nrn format)
 */
class OldSurfaceSynapseGroup : public SynapseGroup
{
public:
    void addSynapse(const uint64_t id, const int32_t section,
                    const brayns::Vector3f& position);

    void mapToCell(const MorphologyInstance&) final;
    void mapSimulation(const std::unordered_map<uint64_t, uint64_t>&) final;
    SynapseMaterialMap::Ptr addToModel(brayns::Model& model) const final;

private:
    std::vector<uint64_t> _ids;
    std::vector<int32_t> _sections;
    std::vector<brayns::Vector3f> _positions;

    // Holds the indices of the synapses that were sucessfully converted to
    // geometry
    std::vector<size_t> _addedSynapses;
    std::vector<brayns::SDFGeometry> _geometry;
};
