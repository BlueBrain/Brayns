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

#include "CellGrowthHandler.h"

CellGrowthHandler::CellGrowthHandler(const uint32_t nbFrames)
    : brayns::AbstractSimulationHandler()
{
    // Load simulation information from compartment reports
    _startTime = 0.0;
    _endTime = static_cast<double>(nbFrames);
    _dt = 1.f;
    _nbFrames = nbFrames;
    _unit = "microns";
    _frameSize = nbFrames;
}

CellGrowthHandler::CellGrowthHandler(const CellGrowthHandler& rhs)
    : brayns::AbstractSimulationHandler(rhs)
{
}

CellGrowthHandler::~CellGrowthHandler() {}

void* CellGrowthHandler::getFrameDataImpl(const uint32_t frame)
{
    if (_currentFrame != frame)
    {
        _frameData.resize(_frameSize);
        for (uint64_t i = 0; i < _frameSize; ++i)
            _frameData[i] = (i < frame ? i : _frameSize);
        _currentFrame = frame;
    }
    return _frameData.data();
}

brayns::AbstractSimulationHandlerPtr CellGrowthHandler::clone() const
{
    return std::make_shared<CellGrowthHandler>(*this);
}
