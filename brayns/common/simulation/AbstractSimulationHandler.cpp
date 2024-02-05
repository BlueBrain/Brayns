/* Copyright 2015-2024 Blue Brain Project/EPFL
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
AbstractSimulationHandler& AbstractSimulationHandler::operator=(
    const AbstractSimulationHandler& rhs)
{
    if (this == &rhs)
        return *this;

    _currentFrame = rhs._currentFrame;
    _nbFrames = rhs._nbFrames;
    _frameSize = rhs._frameSize;
    _startTime = rhs._startTime;
    _endTime = rhs._endTime;
    _dt = rhs._dt;
    _frameAdjuster = rhs._frameAdjuster;
    _unit = rhs._unit;
    _frameData = rhs._frameData;

    return *this;
}

void* AbstractSimulationHandler::getFrameData(const uint32_t frame)
{
    if (frame != _currentFrame)
    {
        _frameData = getFrameDataImpl(getBoundedFrame(frame));
        _currentFrame = frame;
    }

    return _frameData.data();
}

uint32_t AbstractSimulationHandler::getBoundedFrame(
    const uint32_t inputFrame) const
{
    const auto frame = static_cast<uint32_t>(inputFrame * _frameAdjuster);
    const double frameTimestamp = static_cast<double>(frame) * _dt;

    if (frameTimestamp <= _startTime)
        return 0u;
    else if (frameTimestamp >= _endTime)
        return _nbFrames > 0u ? _nbFrames - 1 : 0u;
    else
        return static_cast<uint32_t>(frameTimestamp / _dt);
}
} // namespace brayns
