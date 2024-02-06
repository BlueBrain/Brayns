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

#include "SimulationScanner.h"

#include <brayns/engine/components/SimulationInfo.h>

namespace
{
class SimulationUpdater
{
public:
    static void update(brayns::SimulationParameters &simulation, double start, double end, double dt)
    {
        dt = std::nextafter(dt, std::numeric_limits<double>::infinity());

        _checkFrameLimits(start, end, dt);

        auto startFrame = static_cast<uint32_t>(start / dt);
        auto endFrame = static_cast<uint32_t>(end / dt);

        simulation.setStartFrame(startFrame);
        simulation.setEndFrame(endFrame);
        simulation.setDt(dt);

        auto currentFrame = simulation.getFrame();
        currentFrame = std::min(endFrame, std::max(startFrame, currentFrame));
        simulation.setFrame(currentFrame);
    }

private:
    static void _checkFrameLimits(double start, double end, double dt)
    {
        if (start / dt > static_cast<double>(std::numeric_limits<uint32_t>::max()))
        {
            throw std::runtime_error("Start frame is greater than 2^32");
        }
        if (end / dt > static_cast<double>(std::numeric_limits<uint32_t>::max()))
        {
            throw std::runtime_error("End frame is greater than 2^32");
        }
    }
};
}

namespace brayns
{
void SimulationScanner::scanAndUpdate(ModelManager &models, SimulationParameters &globalSimulation)
{
    auto earlierStart = std::numeric_limits<double>::max();
    auto latestEnd = std::numeric_limits<double>::lowest();
    auto smallestDt = std::numeric_limits<double>::max();
    bool foundSimulation{false};

    auto &instances = models.getAllModelInstances();
    for (auto &instance : instances)
    {
        auto &model = instance->getModel();
        auto &components = model.getComponents();
        auto simulation = components.find<SimulationInfo>();

        if (!simulation)
        {
            continue;
        }
        foundSimulation = true;

        auto startTime = simulation->startTime;
        auto endTime = simulation->endTime;
        auto dt = simulation->dt;

        earlierStart = std::min(startTime, earlierStart);
        latestEnd = std::max(endTime, latestEnd);
        smallestDt = std::min(dt, smallestDt);
    }

    // Update only if we actually have any simulaiton
    if (!foundSimulation)
    {
        globalSimulation.reset();
        return;
    }
    SimulationUpdater::update(globalSimulation, earlierStart, latestEnd, smallestDt);
}
}
