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

#include "SimulationHandler.h"

#include <brayns/common/log.h>
#include <brayns/common/material/Material.h>
#include <brayns/parameters/ApplicationParameters.h>
#include <brayns/parameters/GeometryParameters.h>

#include <brain/compartmentReport.h>
#include <brain/compartmentReportMapping.h>

#include <servus/types.h>

namespace brayns
{
SimulationHandler::SimulationHandler(
    const CompartmentReportPtr& compartmentReport, const bool synchronousMode,
    const double startTime, const double endTime, const double simulationStep)
    : _compartmentReport(compartmentReport)
    , _synchronousMode(synchronousMode)
{
    // Load simulation information from compartment reports
    const auto& metadata = compartmentReport->getReader().getMetaData();
    _startTime = std::max(metadata.startTime, startTime);
    _endTime = std::min(metadata.endTime, endTime);
    _dt = std::max(metadata.timeStep, simulationStep);
    _unit = metadata.timeUnit;
    _frameSize = _compartmentReport->getMapping().getFrameSize();
    _nbFrames = (_endTime - _startTime) / _dt;

    BRAYNS_INFO << "-----------------------------------------------------------"
                << std::endl;
    BRAYNS_INFO << "Simulation information" << std::endl;
    BRAYNS_INFO << "----------------------" << std::endl;
    BRAYNS_INFO << "Start frame          : " << _startTime << "/"
                << metadata.startTime << std::endl;
    BRAYNS_INFO << "End frame            : " << _endTime << "/"
                << metadata.endTime << std::endl;
    BRAYNS_INFO << "Steps between frames : " << _dt << "/" << metadata.timeStep
                << std::endl;
    BRAYNS_INFO << "Number of frames : " << _nbFrames << std::endl;
    BRAYNS_INFO << "-----------------------------------------------------------"
                << std::endl;
}

SimulationHandler::~SimulationHandler()
{
    for (const auto& material : _materials)
        material->setCurrentType("default");
}

void SimulationHandler::bind(const MaterialPtr& material)
{
    material->setCurrentType("simulation");
    _materials.push_back(material);
}

void SimulationHandler::unbind(const MaterialPtr& material)
{
    auto i = std::find(_materials.begin(), _materials.end(), material);
    if (i != _materials.end())
    {
        material->setCurrentType("default");
        _materials.erase(i);
    }
}

bool SimulationHandler::isReady() const
{
    return _ready;
}

void SimulationHandler::waitReady() const
{
    if (_currentFrameFuture.valid())
        _currentFrameFuture.wait();
}

void* SimulationHandler::getFrameData(uint32_t frame)
{
    frame = _getBoundedFrame(frame);

    if (!_currentFrameFuture.valid() && _currentFrame != frame)
        _triggerLoading(frame);

    if (!_makeFrameReady(frame))
        return nullptr;

    return _frameData.data();
}

void SimulationHandler::_triggerLoading(const uint32_t frame)
{
    auto timestamp = _startTime + frame * _dt;
    timestamp = std::max(_startTime, timestamp);
    timestamp = std::min(_endTime, timestamp);

    if (_currentFrameFuture.valid())
        _currentFrameFuture.wait();

    _ready = false;
    _currentFrameFuture = _compartmentReport->load(timestamp);
}

bool SimulationHandler::_isFrameLoaded() const
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

bool SimulationHandler::_makeFrameReady(const uint32_t frame)
{
    if (_isFrameLoaded())
    {
        try
        {
            _frameData = std::move(*_currentFrameFuture.get().data);
        }
        catch (const std::exception& e)
        {
            BRAYNS_ERROR << "Error loading simulation frame " << frame << ": "
                         << e.what() << std::endl;
            return false;
        }
        _currentFrame = frame;
        _ready = true;
    }
    return true;
}
}
