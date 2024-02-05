/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/common/Log.h>

namespace brayns
{
AnimationParameters::AnimationParameters()
    : AbstractParameters("Animation")
{
}

void AnimationParameters::print()
{
    AbstractParameters::print();
    Log::info("Animation frame          : {}.", _current);
}

void AnimationParameters::reset()
{
    _updateValue(_current, 0u, false);
    _updateValue(_dt, 0., false);
    _updateValue(_startFrame, 0u, false);
    _updateValue(_endFrame, 0u, false);

    // trigger the modified callback only once
    if (isModified())
        markModified();
}

void AnimationParameters::setStartAndEndFrame(const uint32_t startFrame,
                                              const uint32_t endFrame) noexcept
{
    _updateValue(_startFrame, startFrame);
    _updateValue(_endFrame, endFrame);
    // Recompute current with the new start and end fraems
    setFrame(_current);
}

uint32_t AnimationParameters::getStartFrame() const noexcept
{
    return _startFrame;
}

uint32_t AnimationParameters::getEndFrame() const noexcept
{
    return _endFrame;
}

void AnimationParameters::setFrame(const uint32_t value) noexcept
{
    const auto numFrames = _endFrame - _startFrame;
    auto newValue =
        numFrames == 0 ? 0 : (value >= numFrames ? numFrames - 1 : value);
    _updateValue(_current, newValue);
}

uint32_t AnimationParameters::getFrame() const noexcept
{
    return _current;
}

uint32_t AnimationParameters::getAbsoluteFrame() const noexcept
{
    return _startFrame + _current;
}

void AnimationParameters::setDt(const double dt) noexcept
{
    _updateValue(_dt, dt);
}

double AnimationParameters::getDt() const noexcept
{
    return _dt;
}

const std::string& AnimationParameters::getTimeUnit() const noexcept
{
    return _unit;
}

} // namespace brayns
