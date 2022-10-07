/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/engine/components/Geometries.h>
#include <brayns/engine/components/SimulationInfo.h>
#include <brayns/engine/geometry/types/Capsule.h>

#include <components/RadiiReportData.h>

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

class SimulationChecks
{
public:
    static bool enabled(brayns::SimulationInfo &info, RadiiReportData &report, brayns::Components &components)
    {
        if (info.enabled)
        {
            return true;
        }
        if (report.lastEnabledFlag)
        {
            RadiiSetter::fromVector(components, report.originalRadii);
        }
        report.lastEnabledFlag = false;
        return false;
    }

    static bool mustUpdate(RadiiReportData &report, const brayns::SimulationParameters &sim)
    {
        auto flagModified = !report.lastEnabledFlag;
        report.lastEnabledFlag = true;
        auto simulationModified = sim.isModified();
        return flagModified || simulationModified;
    }
};
}

void RadiiReportSystem::execute(const brayns::ParametersManager &parameters, brayns::Components &components)
{
    auto &info = components.get<brayns::SimulationInfo>();
    auto &report = components.get<RadiiReportData>();
    if (!SimulationChecks::enabled(info, report, components))
    {
        return;
    }

    auto &simulation = parameters.getSimulationParameters();
    if (!SimulationChecks::mustUpdate(report, simulation))
    {
        return;
    }

    auto frameIndex = simulation.getFrame();
    auto frameData = report.data->getFrame(frameIndex);
    RadiiSetter::fromFrame(components, report.offsets, frameData);
}