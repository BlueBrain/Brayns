/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns
 * <https://github.com/BlueBrain/Brayns-UC-DTI>
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

#include "DTISimulationHandler.h"

#include <brayns/common/Log.h>
#include <brayns/common/Transformation.h>
#include <brayns/common/utils/utils.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <brayns/parameters/AnimationParameters.h>
#include <brayns/parameters/GeometryParameters.h>
#include <brayns/parameters/ParametersManager.h>

#include <algorithm>
#include <fstream>

namespace
{
const size_t DEFAULT_NB_FRAMES = 1000;
const float DEFAULT_TIME_INTERVAL = 1.f;
} // namespace

namespace dti
{
DTISimulationHandler::DTISimulationHandler(
    const Indices& indices, const SetSpikeSimulationMessage& spikeSimulation)
    : brayns::AbstractSimulationHandler()
    , _indices(indices)
    , _spikeSimulation(spikeSimulation)
{
    _frameSize = indices[indices.size() - 1];
    _startTime = 0.0;
    _endTime = _spikeSimulation.end_time;
    _dt = _spikeSimulation.dt;
    _nbFrames = _endTime / _dt;
    _unit = "ms";

    _data.resize(_frameSize, 0.f);

    // Load initial frame
    getFrameData(0);

    brayns::Log::info("[DTI] -----------------------------------------------");
    brayns::Log::info("[DTI] Spike simulation information");
    brayns::Log::info("[DTI] ----------------------");
    brayns::Log::info("[DTI] End time             : {}", _endTime);
    brayns::Log::info("[DTI] Number of frames     : {}", _nbFrames);
    brayns::Log::info("[DTI] -----------------------------------------------");
}

void* DTISimulationHandler::getFrameDataImpl(const uint32_t frame)
{
    if (_currentFrame == frame)
        return (void*)_data.data();

    _currentFrame = frame;
    uint64_t begin = 0;
    for (uint64_t i = 0; i < _indices.size(); ++i)
    {
        const auto end = _indices[i];

        for (uint64_t j = begin; j < end; ++j)
        {
            const float timestamp =
                (j - begin) + (_spikes[_spikeSimulation.gids[i]] / _dt *
                               _spikeSimulation.time_scale);
            float value = _spikeSimulation.rest_intensity;
            if (frame > timestamp)
            {
                value =
                    _spikeSimulation.rest_intensity +
                    _spikeSimulation.spike_intensity -
                    _spikeSimulation.spike_intensity *
                        std::min(1.f,
                                 std::max(static_cast<float>(
                                              _spikeSimulation.decay_speed) *
                                              (frame - timestamp),
                                          0.f));
            }
            _data[j] = std::max(0.f, std::min(value, 1.f));
        }
        begin = end + 1;
    }
    return (void*)_data.data();
}

brayns::AbstractSimulationHandlerPtr DTISimulationHandler::clone() const
{
    return std::make_shared<DTISimulationHandler>(*this);
}
} // namespace dti
