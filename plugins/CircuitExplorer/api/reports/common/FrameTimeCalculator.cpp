/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include "FrameTimeCalculator.h"

#include <cmath>

float FrameTimeCalculator::compute(uint32_t frame, float start, float end, float dt) noexcept
{
    auto upRoundedDt = std::nextafter(dt, std::numeric_limits<float>::infinity());
    auto timeStamp = start + frame * upRoundedDt;

    if (timeStamp < start)
    {
        return start;
    }

    if (timeStamp >= end)
    {
        return end - upRoundedDt;
    }

    return timeStamp;
}
