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

#include "EndFootGroup.h"

#include <plugin/api/MaterialUtils.h>
#include <plugin/io/synapse/SynapseMaterialMap.h>

void EndFootGroup::addSynapse(const uint64_t,
                              brayns::TriangleMesh&& endFootMesh)
{
    _meshes.emplace_back(std::move(endFootMesh));
}

void EndFootGroup::mapToCell(const MorphologyInstance&)
{
    // nothing to do for endfeet
}

void EndFootGroup::mapSimulation(const std::unordered_map<uint64_t, uint64_t>&)
{
    // mesh simulation not supported yet
}

SynapseMaterialMap::Ptr EndFootGroup::addToModel(brayns::Model& model) const
{
    auto result = std::make_unique<SynapseMaterialMap>();
    result->materials.reserve(_meshes.size());
    for (auto& mesh : _meshes)
    {
        const auto matId = CircuitExplorerMaterial::create(model);
        model.getTriangleMeshes()[matId] = std::move(mesh);
        result->materials.push_back({0u, matId});
    }
    return result;
}
