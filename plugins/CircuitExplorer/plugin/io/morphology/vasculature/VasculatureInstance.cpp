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

#include "VasculatureInstance.h"

#include <plugin/io/morphology/vasculature/VasculatureComponent.h>

#include <brayns/engine/Model.h>

VasculatureInstance::VasculatureInstance(
    const brayns::Vector3f &start,
    const float startR,
    const brayns::Vector3f &end,
    const float endR,
    const VasculatureSection sectionType)
    : _geometry(brayns::Primitive::cone(start, startR, end, endR))
    , _sectionType(sectionType)
{
}

std::vector<uint64_t> VasculatureInstance::mapSimulation(const SimulationMapping &mapping) const
{
    return {mapping.globalOffset};
}

void VasculatureInstance::addToModel(uint64_t id, brayns::Model &model)
{
    auto &vasculature = model.getComponent<VasculatureComponent>();
    vasculature.addVessel(id, _geometry, _sectionType);
}

size_t VasculatureInstance::getSectionSegmentCount(const int32_t) const
{
    return 1;
}

MorphologyInstance::SegmentPoints VasculatureInstance::getSegment(const int32_t, const uint32_t) const
{
    return std::make_pair(&_geometry.p0, &_geometry.p1);
}
