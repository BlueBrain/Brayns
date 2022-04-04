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

#include "SimulationScanner.h"

#include <brayns/engine/components/SimulationComponent.h>

namespace
{
struct AnimationUpdater
{
    static void update(brayns::AnimationParameters& animation, float start, float end, float dt)
    {
        dt = std::nextafter(dt, std::numeric_limits<float>::infinity());
        const auto startFrame = uint32_t(start / dt);
        const auto endFrame = uint32_t(end / dt);

        animation.setStartFrame(startFrame);
        animation.setEndFrame(endFrame);
        animation.setDt(dt);

        auto currentFrame = animation.getFrame();
        currentFrame = std::min(endFrame, std::max(startFrame, currentFrame));
        animation.setFrame(currentFrame);
    }
};
}

namespace brayns
{
void SimulationScanner::scanAndUpdate(SceneModelManager &modelManager, AnimationParameters &globalAnimation)
{
    float earlierStart = std::numeric_limits<float>::max();
    float latestEnd = std::numeric_limits<float>::lowest();
    float smallestDt = std::numeric_limits<float>::max();
    bool foundSimulation {false};

    auto &instances = modelManager.getAllModelInstances();
    for(auto instancePtr : instances)
    {
        auto &model = instancePtr->getModel();
        SimulationComponent *simulation = nullptr;

        try
        {
            auto &component = model.getComponent<SimulationComponent>();
            simulation = &component;
            foundSimulation = true;
        }
        catch(...)
        {
            continue;
        }

        auto startTime = simulation->getStartTime();
        auto endTime = simulation->getEndTime();
        auto dt = simulation->getDT();

        earlierStart = startTime < earlierStart ? startTime : earlierStart;
        latestEnd = endTime > latestEnd ? endTime : latestEnd;
        smallestDt = dt < smallestDt ? dt : smallestDt;
    }

    // Update only if we actually have any simulaiton
    if(foundSimulation)
    {
        AnimationUpdater::update(globalAnimation, earlierStart, latestEnd, smallestDt);
    }
}
}
