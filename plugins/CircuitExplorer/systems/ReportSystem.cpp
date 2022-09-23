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

#include "ReportSystem.h"

#include <brayns/common/ColorRamp.h>
#include <brayns/engine/components/SimulationInfo.h>

#include <api/reports/ColorRampUtils.h>
#include <components/Coloring.h>
#include <components/ReportData.h>

namespace
{
class SimulationChecks
{
public:
    static bool enabled(brayns::SimulationInfo &info, ReportData &report)
    {
        if (!info.enabled)
        {
            report.lastEnabledFlag = false;
            return false;
        }
        return true;
    }

    static bool mustUpdate(ReportData &report, brayns::ColorRamp &colorRamp, const brayns::SimulationParameters &sim)
    {
        auto flagModified = !report.lastEnabledFlag;
        report.lastEnabledFlag = true;
        auto colorRampModified = colorRamp.isModified();
        colorRamp.resetModified();
        auto simulationModified = sim.isModified();
        return flagModified || colorRampModified || simulationModified;
    }
};
}

void ReportSystem::execute(const brayns::ParametersManager &parameters, brayns::Components &components)
{
    auto &info = components.get<brayns::SimulationInfo>();
    auto &report = components.get<ReportData>();
    if (!SimulationChecks::enabled(info, report))
    {
        return;
    }

    auto &colorRamp = components.get<brayns::ColorRamp>();
    auto &simulation = parameters.getSimulationParameters();
    if (!SimulationChecks::mustUpdate(report, colorRamp, simulation))
    {
        return;
    }

    auto colors = ColorRampUtils::createSampleBuffer(colorRamp);

    const auto frameIndex = simulation.getFrame();
    const auto frameData = report.data->getFrame(frameIndex);
    const auto &range = colorRamp.getValuesRange();
    auto indices = report.indexer->generate(frameData, range);

    auto &coloring = components.get<Coloring>();
    coloring.painter->updateIndexedColor(colors, indices);
}
