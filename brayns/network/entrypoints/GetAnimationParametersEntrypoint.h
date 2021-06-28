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
#include <brayns/parameters/ParametersManager.h>

namespace brayns
{
BRAYNS_MESSAGE_BEGIN(GetAnimationParametersResult)
BRAYNS_MESSAGE_ENTRY(uint32_t, frame_count, "Animation frame count");
BRAYNS_MESSAGE_ENTRY(uint32_t, current, "Current frame index");
BRAYNS_MESSAGE_ENTRY(int32_t, delta, "Frame delta");
BRAYNS_MESSAGE_ENTRY(double, dt, "Frame time");
BRAYNS_MESSAGE_ENTRY(bool, playing, "Animation is playing");
BRAYNS_MESSAGE_ENTRY(std::string, unit, "Time unit");
BRAYNS_MESSAGE_END()

class GetAnimationParametersEntrypoint
    : public Entrypoint<JsonValue, GetAnimationParametersResult>
{
public:
    virtual std::string getName() const override {
        return "get-animation-parameters";
    }

    virtual std::string getDescription() const override
    {
        return "Get the current state of the animation parameters";
    }

    virtual void onRequest(const Request& request) const override
    {
        auto& manager = getApi().getParametersManager();
        auto& parameters = manager.getAnimationParameters();
        GetAnimationParametersResult result;
        result.frame_count = parameters.getNumFrames();
        result.current = parameters.getFrame();
        result.delta = parameters.getDelta();
        result.dt = parameters.getDt();
        result.playing = parameters.isPlaying();
        result.unit = parameters.getUnit();
        request.reply(result);
    }
};
} // namespace brayns