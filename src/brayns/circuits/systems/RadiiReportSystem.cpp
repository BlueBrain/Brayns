/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#include "RadiiReportSystem.h"

#include <utility>

#include <brayns/core/engine/components/Geometries.h>
#include <brayns/core/engine/components/SimulationInfo.h>
#include <brayns/core/engine/geometry/types/Capsule.h>

#include <brayns/circuits/components/RadiiReportData.h>

namespace
{
class RadiiSetter
{
public:
    static void fromVector(brayns::Components &components, const std::vector<float> &radii)
    {
        auto &geometry = _getGeometry(components);
        size_t i = 0;
        geometry.forEach(
            [&](brayns::Capsule &primitive)
            {
                auto index = i++ * 2;
                primitive.r0 = radii[index];
                primitive.r1 = radii[index + 1];
            });
    }

    static void fromFrame(brayns::Components &components, std::vector<size_t> &offsets, std::vector<float> &frame)
    {
        auto &geometry = _getGeometry(components);
        size_t i = 0;
        geometry.forEach(
            [&](brayns::Capsule &primitive)
            {
                const auto offset = offsets[i++];
                primitive.r0 = frame[offset];
                primitive.r1 = frame[offset];
            });
    }

private:
    static brayns::Geometry &_getGeometry(brayns::Components &components)
    {
        auto &geometries = components.get<brayns::Geometries>();
        assert(geometries.elements.size() == 1);
        return geometries.elements.back();
    }
};
}

bool RadiiReportSystem::isEnabled(brayns::Components &components)
{
    auto &info = components.get<brayns::SimulationInfo>();

    if (info.enabled)
    {
        return true;
    }

    auto &report = components.get<RadiiReportData>();
    if (std::exchange(report.lastEnabledFlag, false))
    {
        RadiiSetter::fromVector(components, report.originalRadii);
    }

    return false;
}

bool RadiiReportSystem::shouldExecute(brayns::Components &components)
{
    auto &report = components.get<RadiiReportData>();
    return !std::exchange(report.lastEnabledFlag, true);
}

void RadiiReportSystem::execute(brayns::Components &components, double frameTimestamp)
{
    auto &report = components.get<RadiiReportData>();
    auto frameData = report.data->getFrame(frameTimestamp);
    RadiiSetter::fromFrame(components, report.offsets, frameData);
}
