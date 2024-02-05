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

#include "SurfaceSynapseGroup.h"

#include <plugin/api/MaterialUtils.h>
#include <plugin/io/synapse/SynapseMaterialMap.h>

void SurfaceSynapseGroup::addSynapse(const uint64_t id, const int32_t section,
                                     const float distance,
                                     const brayns::Vector3f& position)
{
    _ids.push_back(id);
    _sections.push_back(section);
    _distances.push_back(distance);
    _positions.push_back(position);
}

void SurfaceSynapseGroup::mapToCell(const MorphologyInstance& cell)
{
    for (size_t j = 0; j < _ids.size(); ++j)
    {
        const auto section = _sections[j];
        try
        {
            const auto segmentCount = cell.getSectionSegmentCount(section);
            if (segmentCount == 0)
                continue;

            const auto& surfPos = _positions[j];
            const auto distance = _distances[j];

            float totalDistance = 0.f;
            std::vector<float> localDistances(segmentCount, 0.f);
            for (size_t i = 0; i < segmentCount; ++i)
            {
                const auto points = cell.getSegment(section, i);
                const auto dist = glm::length(*points.first - *points.second);
                totalDistance += dist;
                localDistances[i] = dist;
            }
            const float invTotalDist = 1.f / totalDistance;

            float traversedDistance = 0.f;
            for (size_t i = 0; i < localDistances.size(); ++i)
            {
                traversedDistance += localDistances[i];
                const float localNorm = traversedDistance * invTotalDist;
                if (localNorm >= distance)
                {
                    const auto points = cell.getSegment(section, i);
                    const auto point = glm::lerp(*points.first, *points.second,
                                                 distance / localNorm);
                    const auto dirVector = glm::normalize(surfPos - point);
                    _geometry.push_back(brayns::createSDFConePillSigmoid(
                        point, point + dirVector * 3.5f, 0.35f, 0.5f));
                    _addedSynapses.push_back(j);
                    break;
                }
            }
        }
        catch (...)
        {
            continue;
        }
    }
}

void SurfaceSynapseGroup::mapSimulation(
    const std::unordered_map<uint64_t, uint64_t>& mapping)
{
    for (size_t i = 0; i < _addedSynapses.size(); ++i)
    {
        auto it = mapping.find(_addedSynapses[i]);
        if (it != mapping.end())
            _geometry[_addedSynapses[i]].userData = it->second;
    }
}

SynapseMaterialMap::Ptr SurfaceSynapseGroup::addToModel(
    brayns::Model& model) const
{
    auto result = std::make_unique<SynapseMaterialMap>();
    result->materials.reserve(_addedSynapses.size());
    for (size_t i = 0; i < _addedSynapses.size(); ++i)
    {
        const auto matId = CircuitExplorerMaterial::create(model);
        model.addSDFGeometry(matId, _geometry[i], {});
        result->materials.push_back({_ids[_addedSynapses[i]], matId});
    }
    return result;
}
