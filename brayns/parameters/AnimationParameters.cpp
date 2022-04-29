/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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
    _current = 0u;
    _dt = 0.;
    _startFrame = 0u;
    _endFrame = 0u;
    markModified();
}

void AnimationParameters::setStartFrame(const uint32_t startFrame) noexcept
{
    _updateValue(_startFrame, startFrame);
}

void AnimationParameters::setEndFrame(const uint32_t endFrame) noexcept
{
    _updateValue(_endFrame, endFrame);
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
    _updateValue(_current, value);
}

uint32_t AnimationParameters::getFrame() const noexcept
{
    return _current;
}

void AnimationParameters::setDt(const double dt) noexcept
{
    _updateValue(_dt, dt);
}

double AnimationParameters::getDt() const noexcept
{
    return _dt;
}

const std::string &AnimationParameters::getTimeUnit() const noexcept
{
    return _unit;
}

} // namespace brayns
