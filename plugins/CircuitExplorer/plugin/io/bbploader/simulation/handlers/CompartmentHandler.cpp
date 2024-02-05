/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/BlueBrain/Brayns>
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

#include "CompartmentHandler.h"

#include <brayns/common/Log.h>

namespace
{
auto frameIndexToTimestamp(const uint32_t frame, const double dt) noexcept
{
    return static_cast<double>(std::nextafter(dt, INFINITY) * frame);
}
} // namespace

CompartmentHandler::CompartmentHandler(
    const std::string& path,
    const std::shared_ptr<brion::CompartmentReport>& report)
    : brayns::AbstractSimulationHandler()
    , _path(path)
    , _report(report)
{
    _startTime = _report->getStartTime();
    _endTime = _report->getEndTime();
    _dt = _report->getTimestep();
    _nbFrames = (_endTime - _startTime) / _dt;
    _unit = _report->getTimeUnit();
    _frameSize = _report->getFrameSize();
}

CompartmentHandler::CompartmentHandler(const CompartmentHandler& o)
    : brayns::AbstractSimulationHandler(o)
    , _path(o._path)
    , _report(o._report)
    , _ready(false)
{
}

brayns::AbstractSimulationHandlerPtr CompartmentHandler::clone() const
{
    return std::make_shared<CompartmentHandler>(*this);
}

bool CompartmentHandler::isReady() const
{
    return _ready;
}

std::vector<float> CompartmentHandler::getFrameDataImpl(const uint32_t frame)
{
    std::vector<float> data;
    _ready = false;
    auto loadFuture =
        _report->loadFrame(frameIndexToTimestamp(frame, _dt) + _startTime);
    if (loadFuture.valid())
    {
        loadFuture.wait();
        auto frameData = loadFuture.get();
        if (frameData.data.get() != nullptr)
        {
            data = std::move(*frameData.data);
            _ready = true;
        }
        else
            brayns::Log::warn("[CE] Attempt to get frame from {} failed.",
                              _path);
    }
    return data;
}
