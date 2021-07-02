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
#include <brayns/network/messages/AnimationParametersMessage.h>

#include <brayns/parameters/AnimationParameters.h>

namespace brayns
{
class GetAnimationParametersEntrypoint
    : public Entrypoint<EmptyMessage, AnimationParametersMessage>
{
public:
    virtual std::string getName() const override
    {
        return "get-animation-parameters";
    }

    virtual std::string getDescription() const override
    {
        return "Get the current state of the animation parameters";
    }

    virtual void onCreate() override
    {
        auto& manager = getApi().getParametersManager();
        auto& animationParameters = manager.getAnimationParameters();
        animationParameters.onModified(
            [this](const auto& object)
            {
                auto params = extractAnimationParameters();
                notify(params);
            });
    }

    virtual void onRequest(const Request& request) const override
    {
        auto result = extractAnimationParameters();
        request.reply(result);
    }

private:
    AnimationParametersMessage extractAnimationParameters() const
    {
        auto& manager = getApi().getParametersManager();
        auto& animationParameters = manager.getAnimationParameters();
        AnimationParametersMessage result;
        result.frame_count = animationParameters.getNumFrames();
        result.current = animationParameters.getFrame();
        result.delta = animationParameters.getDelta();
        result.dt = animationParameters.getDt();
        result.playing = animationParameters.isPlaying();
        result.unit = animationParameters.getUnit();
        return result;
    }
};

class SetAnimationParametersEntrypoint
    : public Entrypoint<AnimationParametersMessage, EmptyMessage>
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
        auto& animationParameters = manager.getAnimationParameters();
        animationParameters.setNumFrames(params.frame_count);
        animationParameters.setFrame(params.current);
        animationParameters.setDelta(params.delta);
        animationParameters.setDt(params.dt);
        animationParameters.setPlaying(params.playing);
        animationParameters.setUnit(params.unit);
        request.reply(EmptyMessage());
    }
};
} // namespace brayns