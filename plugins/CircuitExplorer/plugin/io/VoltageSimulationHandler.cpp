/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#include "VoltageSimulationHandler.h"

#include <brayns/common/Log.h>
#include <brayns/parameters/AnimationParameters.h>

#include <brion/types.h>

VoltageSimulationHandler::VoltageSimulationHandler(
    const std::string& reportPath, const brion::GIDSet& gids,
    const bool synchronousMode)
    : brayns::AbstractSimulationHandler()
    , _synchronousMode(synchronousMode)
    , _reportPath(reportPath)
    , _compartmentReport(new brion::CompartmentReport(brion::URI(reportPath),
                                                      brion::MODE_READ, gids))
{
    // Load simulation information from compartment reports
    _startTime = _compartmentReport->getStartTime();
    _endTime = _compartmentReport->getEndTime();
    _dt = _compartmentReport->getTimestep();
    _nbFrames = _endTime / _dt;
    _unit = _compartmentReport->getTimeUnit();
    _frameSize = _compartmentReport->getFrameSize();

    brayns::Log::info(
        "[CE] -----------------------------------------------------------");
    brayns::Log::info("[CE] Voltage simulation information");
    brayns::Log::info("[CE] ----------------------");
    brayns::Log::info("[CE] Start time           : {}", _startTime);
    brayns::Log::info("[CE] End time             : {}", _endTime);
    brayns::Log::info("[CE] Steps between frames : {}", _dt);
    brayns::Log::info("[CE] Number of frames     : {}", _nbFrames);
    brayns::Log::info("[CE] Frame size           : {}", _frameSize);
    brayns::Log::info("[CE] Mode                 : {}",
                      (_synchronousMode ? "Synchronous" : "Asynchronous"));
    brayns::Log::info(
        "[CE] -----------------------------------------------------------");
}

VoltageSimulationHandler::VoltageSimulationHandler(
    const VoltageSimulationHandler& rhs)
    : brayns::AbstractSimulationHandler(rhs)
    , _synchronousMode(rhs._synchronousMode)
    , _compartmentReport(rhs._compartmentReport)
    , _ready(false)
{
}

VoltageSimulationHandler::~VoltageSimulationHandler() {}

brayns::AbstractSimulationHandlerPtr VoltageSimulationHandler::clone() const
{
    return std::make_shared<VoltageSimulationHandler>(*this);
}

bool VoltageSimulationHandler::isReady() const
{
    return _ready;
}

void* VoltageSimulationHandler::getFrameDataImpl(const uint32_t frame)
{
    if (!_currentFrameFuture.valid() && _currentFrame != frame)
        _triggerLoading(frame);

    if (!_makeFrameReady(frame))
        return nullptr;

    return _frameData.data();
}

void VoltageSimulationHandler::_triggerLoading(const uint32_t frame)
{
    float timestamp = frame * _dt + _compartmentReport->getStartTime();
    timestamp = std::min(static_cast<float>(_compartmentReport->getEndTime()),
                         timestamp);

    if (_currentFrameFuture.valid())
        _currentFrameFuture.wait();

    _ready = false;
    _currentFrameFuture = _compartmentReport->loadFrame(timestamp);
}

bool VoltageSimulationHandler::_isFrameLoaded() const
{
    if (!_currentFrameFuture.valid())
        return false;

    if (_synchronousMode)
    {
        _currentFrameFuture.wait();
        return true;
    }

    return _currentFrameFuture.wait_for(std::chrono::milliseconds(0)) ==
           std::future_status::ready;
}

bool VoltageSimulationHandler::_makeFrameReady(const uint32_t frame)
{
    if (_isFrameLoaded())
    {
        try
        {
            _frameData = std::move(*_currentFrameFuture.get().data);
        }
        catch (const std::exception& e)
        {
            brayns::Log::error("[CE] Error loading simulation frame {}: {}.",
                               frame, e.what());
            return false;
        }
        _currentFrame = frame;
        _ready = true;
    }
    return true;
}
