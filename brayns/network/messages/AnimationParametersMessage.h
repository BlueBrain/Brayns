/* Copyright (c) 2021 EPFL/Blue Brain Project
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#pragma once

#include <brayns/network/message/Message.h>

#include <brayns/parameters/AnimationParameters.h>

namespace brayns
{
BRAYNS_MESSAGE_BEGIN(AnimationParametersMessage)
BRAYNS_MESSAGE_ENTRY(uint32_t, frame_count, "Animation frame count")
BRAYNS_MESSAGE_ENTRY(uint32_t, current, "Current frame index")
BRAYNS_MESSAGE_ENTRY(int32_t, delta, "Frame delta")
BRAYNS_MESSAGE_ENTRY(double, dt, "Frame time")
BRAYNS_MESSAGE_ENTRY(bool, playing, "Animation is playing")
BRAYNS_MESSAGE_ENTRY(std::string, unit, "Time unit")

static AnimationParameters& extract(PluginAPI& api)
{
    auto& parametersManager = api.getParametersManager();
    return parametersManager.getAnimationParameters();
}

void dump(AnimationParameters& animationParameters) const
{
    animationParameters.setNumFrames(frame_count);
    animationParameters.setFrame(current);
    animationParameters.setDelta(delta);
    animationParameters.setDt(dt);
    animationParameters.setPlaying(playing);
    animationParameters.setUnit(unit);
}

void load(const AnimationParameters& animationParameters)
{
    frame_count = animationParameters.getNumFrames();
    current = animationParameters.getFrame();
    delta = animationParameters.getDelta();
    dt = animationParameters.getDt();
    playing = animationParameters.isPlaying();
    unit = animationParameters.getUnit();
}

BRAYNS_MESSAGE_END()
} // namespace brayns