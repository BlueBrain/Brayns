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

#include <brayns/network/entrypoint/Entrypoint.h>

namespace brayns
{
BRAYNS_MESSAGE_BEGIN(SetAnimationParametersParams)
BRAYNS_MESSAGE_ENTRY(uint32_t, frame_count, "Animation frame count");
BRAYNS_MESSAGE_ENTRY(uint32_t, current, "Current frame index");
BRAYNS_MESSAGE_ENTRY(int32_t, delta, "Frame delta");
BRAYNS_MESSAGE_ENTRY(double, dt, "Frame time");
BRAYNS_MESSAGE_ENTRY(bool, playing, "Animation is playing");
BRAYNS_MESSAGE_ENTRY(std::string, unit, "Time unit");
BRAYNS_MESSAGE_END()

class SetAnimationParametersEntrypoint
    : public Entrypoint<SetAnimationParametersParams, NoResult>
{
public:
    virtual std::string getName() const override
    {
        return "set-animation-parameters";
    }

    virtual std::string getDescription() const override
    {
        return "Set the current state of the animation parameters";
    }

    virtual void onRequest(const Request& request) const override
    {
        auto& params = request.getParams();
        auto& manager = getApi().getParametersManager();
        auto& animation = manager.getAnimationParameters();
        animation.setFrameCount(params.frame_count);
        animation.setFrame(params.current);
        animation.setDelta(params.delta);
        animation.setDt(params.dt);
        animation.setPlaying(params.playing);
        animation.setUnit(params.unit);
        request.reply(result);
    }
};
} // namespace brayns