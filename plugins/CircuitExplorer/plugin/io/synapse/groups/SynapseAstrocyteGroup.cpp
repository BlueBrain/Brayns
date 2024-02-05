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

#include "SynapseAstrocyteGroup.h"

#include <plugin/api/MaterialUtils.h>
#include <plugin/io/synapse/SynapseMaterialMap.h>

void SynapseAstrocyteGroup::addSynapse(const uint64_t id, const int32_t section,
                                       const float distance)
{
    _ids.push_back(id);
    _sections.push_back(section);
    _distances.push_back(distance);
}

void SynapseAstrocyteGroup::mapToCell(const MorphologyInstance& cell)
{
    for (size_t i = 0; i < _ids.size(); ++i)
    {
        const auto section = _sections[i];
        try
        {
            const auto segmentCount = cell.getSectionSegmentCount(section);
            if (segmentCount == 0)
                continue;

            const auto distance = _distances[i];

            float totalDistance = 0.f;
            std::vector<float> localDistances(segmentCount, 0.f);
            for (size_t j = 0; j < segmentCount; ++j)
            {
                const auto points = cell.getSegment(section, j);
                const auto dist = glm::length(*points.first - *points.second);
                totalDistance += dist;
                localDistances[j] = dist;
            }
            const float invTotalDist = 1.f / totalDistance;

            float traversedDistance = 0.f;
            for (size_t j = 0; j < localDistances.size(); ++j)
            {
                traversedDistance += localDistances[j];
                const float localNorm = traversedDistance * invTotalDist;
                if (localNorm >= distance)
                {
                    const auto points = cell.getSegment(section, j);
                    const auto point = glm::lerp(*points.first, *points.second,
                                                 distance / localNorm);
                    _geometry.push_back(brayns::createSDFSphere(point, 2.f));
                    // By default we copy the cell simulation mapping to show
                    // the node simulation report. If there is a synapse report,
                    // it will overwrite this value
                    _geometry.back().userData =
                        cell.getSegmentSimulationOffset(section, j);
                    break;
                }
            }

            _addedSynapses.push_back(i);
        }
        catch (...)
        {
            continue;
        }
    }
}

void SynapseAstrocyteGroup::mapSimulation(
    const std::unordered_map<uint64_t, uint64_t>& mapping)
{
    for (size_t i = 0; i < _addedSynapses.size(); ++i)
    {
        auto it = mapping.find(_addedSynapses[i]);
        if (it != mapping.end())
            _geometry[i].userData = it->second;
    }
}

SynapseMaterialMap::Ptr SynapseAstrocyteGroup::addToModel(
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
