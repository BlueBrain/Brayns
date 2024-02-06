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

#include "SimulationSystem.h"

#include <utility>

#include <brayns/engine/components/SimulationInfo.h>

namespace
{
class SimulationChecks
{
public:
    static bool shouldExecute(const brayns::SimulationParameters &simulation, uint32_t lastFrame)
    {
        return simulation.isModified() && simulation.getFrame() != lastFrame;
    }
};

class FrameTimestamp
{
public:
    static double compute(const brayns::SimulationParameters &simulation, const brayns::SimulationInfo &info)
    {
        auto upRoundedDt = std::nextafter(simulation.getDt(), std::numeric_limits<double>::infinity());
        auto timestamp = simulation.getFrame() * upRoundedDt;

        if (timestamp < info.startTime)
        {
            return info.startTime;
        }

        if (auto upperLimit = info.endTime - upRoundedDt; timestamp >= upperLimit)
        {
            return upperLimit;
        }

        return timestamp;
    }
};
}

namespace brayns
{
void SimulationSystem::execute(const ParametersManager &parameters, Components &components)
{
    if (!isEnabled(components))
    {
        return;
    }

    auto &simulation = parameters.getSimulationParameters();
    auto simulationChanged = SimulationChecks::shouldExecute(simulation, _lastFrame);
    auto stateChanged = shouldExecute(components);
    if (!simulationChanged && !stateChanged)
    {
        return;
    }

    auto &info = components.get<brayns::SimulationInfo>();
    auto timestamp = FrameTimestamp::compute(simulation, info);
    std::exchange(_lastFrame, simulation.getFrame());
    execute(components, timestamp);
}
}
