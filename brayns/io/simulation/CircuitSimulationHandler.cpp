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

#include "CircuitSimulationHandler.h"

#include <brayns/common/log.h>
#include <brayns/parameters/GeometryParameters.h>

#include <servus/types.h>

namespace brayns
{
CircuitSimulationHandler::CircuitSimulationHandler(
    const GeometryParameters& geometryParameters,
    const std::string& reportSource, const brion::GIDSet& gids)
    : AbstractSimulationHandler(geometryParameters)
    , _compartmentReport(nullptr)
{
    _compartmentReport.reset(
        new brion::CompartmentReport(brion::URI(reportSource), brion::MODE_READ,
                                     gids));

    // Load simulation information from compartment reports
    const auto reportStartTime = _compartmentReport->getStartTime();
    const auto reportEndTime = _compartmentReport->getEndTime();
    const auto reportTimeStep = _compartmentReport->getTimestep();

    _beginFrame = std::max(reportStartTime,
                           _geometryParameters.getCircuitStartSimulationTime());
    _endFrame = std::min(reportEndTime,
                         _geometryParameters.getCircuitEndSimulationTime());
    _timeBetweenFrames =
        std::max(reportTimeStep,
                 _geometryParameters.getCircuitSimulationStep());
    _frameSize = _compartmentReport->getFrameSize();
    _nbFrames = (_endFrame - _beginFrame) / _timeBetweenFrames;

    BRAYNS_INFO << "-----------------------------------------------------------"
                << std::endl;
    BRAYNS_INFO << "Simulation information" << std::endl;
    BRAYNS_INFO << "----------------------" << std::endl;
    BRAYNS_INFO << "Start frame          : " << _beginFrame << "/"
                << reportStartTime << std::endl;
    BRAYNS_INFO << "End frame            : " << _endFrame << "/"
                << reportEndTime << std::endl;
    BRAYNS_INFO << "Steps between frames : " << _timeBetweenFrames << "/"
                << reportTimeStep << std::endl;
    BRAYNS_INFO << "Number of frames : " << _nbFrames << std::endl;
    BRAYNS_INFO << "-----------------------------------------------------------"
                << std::endl;
}

CircuitSimulationHandler::~CircuitSimulationHandler()
{
}

void* CircuitSimulationHandler::getFrameData()
{
    if (_compartmentReport)
    {
        auto frame = _beginFrame + _timestamp * _timeBetweenFrames;
        frame = std::min(_endFrame, frame);
        frame = std::max(_beginFrame, frame);
        _frameValues = _compartmentReport->loadFrame(frame).get();
        if (_frameValues)
            return _frameValues.get()->data();
    }
    return nullptr;
}
}
