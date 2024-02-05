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

#include "VasculatureInstance.h"

#include <brayns/engine/Model.h>

#include <plugin/api/MaterialUtils.h>
#include <plugin/io/morphology/vasculature/VasculatureMaterialMap.h>

VasculatureInstance::VasculatureInstance(const brayns::Vector3f& start,
                                         const float startR,
                                         const brayns::Vector3f& end,
                                         const float endR,
                                         const VasculatureSection sectionType)
    : _geometry(
          brayns::createSDFConePill(start, end, startR,
                                    endR)) //_geometry(start, end, startR, endR)
    , _sectionType(sectionType)
{
}

void VasculatureInstance::mapSimulation(const size_t globalOffset,
                                        const std::vector<uint16_t>&,
                                        const std::vector<uint16_t>&)
{
    _geometry.userData = globalOffset;
}

ElementMaterialMap::Ptr VasculatureInstance::addToModel(
    brayns::Model& model) const
{
    const auto newMatId = CircuitExplorerMaterial::create(model);
    model.addSDFGeometry(newMatId, _geometry, {});

    auto materialMap = std::make_unique<VasculatureMaterialMap>();
    materialMap->materialId = newMatId;
    materialMap->sectionType = _sectionType;
    return materialMap;
}

size_t VasculatureInstance::getSectionSegmentCount(const int32_t) const
{
    return 1;
}

MorphologyInstance::SegmentPoints VasculatureInstance::getSegment(
    const int32_t, const uint32_t) const
{
    return std::make_pair(&_geometry.p0, &_geometry.p1);
}

uint64_t VasculatureInstance::getSegmentSimulationOffset(const int32_t,
                                                         const uint32_t) const
{
    return _geometry.userData;
}
