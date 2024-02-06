/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "ReportFactory.h"

#include <brayns/engine/components/SimulationInfo.h>

#include "ColorRampUtils.h"

#include <systems/ReportSystem.h>

namespace
{
class SimulationInfoFactory
{
public:
    static brayns::SimulationInfo create(const IReportData &data)
    {
        auto simInfo = brayns::SimulationInfo();
        simInfo.dt = data.getTimeStep();
        simInfo.endTime = data.getEndTime();
        simInfo.startTime = data.getStartTime();
        simInfo.timeUnit = data.getTimeUnit();
        return simInfo;
    }
};
}

void ReportFactory::create(brayns::Model &model, ReportData reportData)
{
    auto &components = model.getComponents();

    auto simInfo = SimulationInfoFactory::create(*reportData.data);
    components.add<brayns::SimulationInfo>(std::move(simInfo));

    components.add<ReportData>(std::move(reportData));

    auto colorRamp = ColorRampUtils::createUnipolarColorRamp();
    components.add<brayns::ColorRamp>(std::move(colorRamp));

    auto &systems = model.getSystems();
    systems.setUpdateSystem<ReportSystem>();
}
