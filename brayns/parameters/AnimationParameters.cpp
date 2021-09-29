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
} // namespace

namespace brayns
{
AnimationParameters::AnimationParameters()
    : AbstractParameters("Animation")
{
    _parameters.add_options()(PARAM_ANIMATION_FRAME,
                              po::value<uint32_t>(&_current),
                              "Scene animation frame [uint]")(
        PARAM_PLAY_ANIMATION, po::bool_switch(&_playing)->default_value(false),
        "Start animation playback");
}

void AnimationParameters::print()
{
    AbstractParameters::print();
    BRAYNS_INFO << "Animation frame          : " << _current << std::endl;
}

void AnimationParameters::reset()
{
    _updateValue(_current, 0u, false);
    _updateValue(_dt, 0., false);
    _updateValue(_numFrames, 0u, false);
    _updateValue(_playing, false, false);
    _updateValue(_unit, std::string(), false);

    // trigger the modified callback only once
    if (isModified())
        markModified();
}

void AnimationParameters::setDelta(const int32_t delta)
{
    if (delta == 0)
        throw std::logic_error("Animation delta cannot be set to 0");
    _updateValue(_delta, delta);
}

void AnimationParameters::update()
{
    if (_playing && _canUpdateFrame())
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
} // namespace brayns
