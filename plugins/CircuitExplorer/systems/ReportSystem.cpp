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

#include "ReportSystem.h"

#include <utility>

#include <brayns/engine/components/ColorMap.h>
#include <brayns/engine/components/ColorRamp.h>
#include <brayns/engine/components/SimulationInfo.h>

#include <api/reports/ColorRampUtils.h>
#include <components/ColorHandler.h>
#include <components/ReportData.h>

bool ReportSystem::isEnabled(brayns::Components &components)
{
    auto &info = components.get<brayns::SimulationInfo>();
    if (info.enabled)
    {
        return true;
    }

    auto &report = components.get<ReportData>();
    report.lastEnabledFlag = false;

    return false;
}

bool ReportSystem::shouldExecute(brayns::Components &components)
{
    auto &report = components.get<ReportData>();
    auto &colorRamp = components.get<brayns::ColorRamp>();

    auto flagModified = !std::exchange(report.lastEnabledFlag, true);
    auto colorRampModified = colorRamp.isModified();
    colorRamp.resetModified();

    return flagModified || colorRampModified;
}

void ReportSystem::execute(brayns::Components &components, double frameTimestamp)
{
    auto &colorRamp = components.get<brayns::ColorRamp>();
    auto &colorMap = components.getOrAdd<brayns::ColorMap>();

    colorMap.colors = ColorRampUtils::createSampleBuffer(colorRamp);
    auto &range = colorRamp.getValuesRange();

    auto &report = components.get<ReportData>();
    auto frameData = report.data->getFrame(frameTimestamp);
    colorMap.indices = report.indexer->generate(frameData, range);

    auto &painter = *components.get<ColorHandler>().handler;
    auto &geometries = components.get<brayns::Geometries>();
    auto &views = components.get<brayns::GeometryViews>();
    painter.colorByColormap(colorMap, geometries, views);
}
