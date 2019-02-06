/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "AnimationParameters.h"

namespace
{
constexpr auto PARAM_ANIMATION_FRAME = "animation-frame";
constexpr auto PARAM_PLAY_ANIMATION = "play-animation";
}

namespace brayns
{
AnimationParameters::AnimationParameters()
    : AbstractParameters("Animation")
{
    _parameters.add_options()(PARAM_ANIMATION_FRAME, po::value<uint32_t>(),
                              "Scene animation frame [uint]")(
        PARAM_PLAY_ANIMATION, po::bool_switch()->default_value(false),
        "Start animation playback");
}

void AnimationParameters::parse(const po::variables_map& vm)
{
    if (vm.count(PARAM_ANIMATION_FRAME))
        _current = vm[PARAM_ANIMATION_FRAME].as<uint32_t>();
    if (vm[PARAM_PLAY_ANIMATION].as<bool>())
        _delta = 1;
    markModified();
}

void AnimationParameters::print()
{
    AbstractParameters::print();
    BRAYNS_INFO << "Animation frame          : " << _current << std::endl;
}

void AnimationParameters::reset()
{
    _updateValue(_end, 0u, false);
    _updateValue(_current, 0u, false);
    _updateValue(_unit, std::string(), false);
    _updateValue(_dt, 0., false);

    // trigger the modified callback only once
    if (isModified())
        markModified();
}

void AnimationParameters::update()
{
    if ((isModified() || getDelta() != 0) && _canUpdateFrame())
        setFrame(getFrame() + getDelta());
}

void AnimationParameters::jumpFrames(int frames)
{
    if (_canUpdateFrame())
        setFrame(getFrame() + frames);
}

bool AnimationParameters::_canUpdateFrame() const
{
    return !hasIsReadyCallback() || _isReadyCallback();
}
}
