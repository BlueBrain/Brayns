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

#include <brayns/network/adapters/AnimationParametersAdapter.h>
#include <brayns/network/entrypoint/ObjectEntrypoint.h>

namespace brayns
{
template <>
struct ObjectExtractor<AnimationParameters>
{
    static AnimationParameters& extract(PluginAPI& api)
    {
        auto& parametersManager = api.getParametersManager();
        return parametersManager.getAnimationParameters();
    }
};

class GetAnimationParametersEntrypoint
    : public GetEntrypoint<AnimationParameters>
{
public:
    GetAnimationParametersEntrypoint()
    {
        setNotificationPeriod(NotificationPeriod::interactive());
    }

    virtual std::string getName() const override
    {
        return "get-animation-parameters";
    }

    virtual std::string getDescription() const override
    {
        return "Get the current state of the animation parameters";
    }
};

class SetAnimationParametersEntrypoint
    : public SetEntrypoint<AnimationParameters>
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
};
} // namespace brayns