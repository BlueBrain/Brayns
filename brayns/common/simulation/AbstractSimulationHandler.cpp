/* Copyright (c) 2015-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "AbstractSimulationHandler.h"

namespace brayns
{
AbstractSimulationHandler::~AbstractSimulationHandler() = default;

AbstractSimulationHandler& AbstractSimulationHandler::operator=(
    const AbstractSimulationHandler& rhs)
{
    if (this == &rhs)
        return *this;

    _currentFrame = rhs._currentFrame;
    _nbFrames = rhs._nbFrames;
    _frameSize = rhs._frameSize;
    _dt = rhs._dt;
    _unit = rhs._unit;
    _frameData = rhs._frameData;

    return *this;
}

uint32_t AbstractSimulationHandler::_getBoundedFrame(const uint32_t frame) const
{
    const double frameTimestamp = static_cast<double>(frame) * _dt;
    uint32_t boundedFrame = 0;

    if (frameTimestamp <= _startTime)
        boundedFrame = 0;
    else if (frameTimestamp >= _endTime)
        boundedFrame = std::max(_nbFrames - 1, 0u);
    else
    {
        const double realTimestamp = frameTimestamp - _startTime;
        boundedFrame = static_cast<uint32_t>(realTimestamp / _dt);
    }

    return boundedFrame;
}
} // namespace brayns
