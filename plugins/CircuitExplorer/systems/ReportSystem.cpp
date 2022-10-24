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

#include <brayns/engine/components/ColorRamp.h>
#include <brayns/engine/components/SimulationInfo.h>

#include <api/reports/ColorRampUtils.h>
#include <components/Coloring.h>
#include <components/ReportData.h>

namespace
{
class SimulationChecks
{
public:
    static bool shouldExecute(brayns::Components &components)
    {
        auto &report = components.get<ReportData>();
        return _enabled(report, components) && _mustUpdate(report, components);
    }

public:
    static bool _enabled(ReportData &report, brayns::Components &components)
    {
        auto &info = components.get<brayns::SimulationInfo>();
        if (info.enabled)
        {
            return true;
        }

        report.lastEnabledFlag = false;
        return false;
    }

    static bool _mustUpdate(ReportData &report, brayns::Components &components)
    {
        auto flagModified = !std::exchange(report.lastEnabledFlag, true);
        auto &colorRamp = components.get<brayns::ColorRamp>();
        auto colorRampModified = colorRamp.isModified();
        colorRamp.resetModified();
        return flagModified || colorRampModified;
    }
};
}

bool ReportSystem::shouldExecute(brayns::Components &components)
{
    return SimulationChecks::shouldExecute(components);
}

void ReportSystem::execute(brayns::Components &components, uint32_t frame)
{
    auto &colorRamp = components.get<brayns::ColorRamp>();
    auto colors = ColorRampUtils::createSampleBuffer(colorRamp);
    auto &range = colorRamp.getValuesRange();

    auto &report = components.get<ReportData>();
    auto frameData = report.data->getFrame(frame);
    auto indices = report.indexer->generate(frameData, range);

    auto &coloring = components.get<Coloring>();
    coloring.painter->updateIndexedColor(std::move(colors), std::move(indices));
}
