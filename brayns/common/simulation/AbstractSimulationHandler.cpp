/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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
AbstractSimulationHandler::AbstractSimulationHandler(const AbstractSimulationHandler &other)
{
    *this = other;
}

AbstractSimulationHandler &AbstractSimulationHandler::operator=(const AbstractSimulationHandler &rhs)
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

uint32_t AbstractSimulationHandler::getCurrentFrame() const
{
    return _currentFrame;
}

void AbstractSimulationHandler::setCurrentFrame(const uint32_t newFrame)
{
    _currentFrame = newFrame;
}

void AbstractSimulationHandler::setFrameAdjuster(const double adjuster)
{
    _frameAdjuster = adjuster;
}

std::vector<float> AbstractSimulationHandler::getFrameDataImpl(const uint32_t frame)
{
    (void)frame;
    return {};
}

uint64_t AbstractSimulationHandler::getFrameSize() const
{
    return _frameSize;
}

void AbstractSimulationHandler::setFrameSize(const uint64_t frameSize)
{
    _frameSize = frameSize;
}

uint32_t AbstractSimulationHandler::getNbFrames() const
{
    return _nbFrames;
}

void AbstractSimulationHandler::setNbFrames(const uint32_t nbFrames)
{
    _nbFrames = nbFrames;
}

double AbstractSimulationHandler::getStartTime() const
{
    return _startTime;
}

double AbstractSimulationHandler::getEndTime() const
{
    return _endTime;
}

double AbstractSimulationHandler::getDt() const
{
    return _dt;
}

const std::string &AbstractSimulationHandler::getUnit() const
{
    return _unit;
}

bool AbstractSimulationHandler::isReady() const
{
    return true;
}

void AbstractSimulationHandler::waitReady() const
{
}

void *AbstractSimulationHandler::getFrameData(const uint32_t frame)
{
    if (frame != _currentFrame)
    {
        _frameData = getFrameDataImpl(getBoundedFrame(frame));
        _currentFrame = frame;
    }

    return _frameData.data();
}

uint32_t AbstractSimulationHandler::getBoundedFrame(const uint32_t inputFrame) const
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
