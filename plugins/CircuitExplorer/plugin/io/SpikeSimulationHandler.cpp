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

#include <common/log.h>

#include "SpikeSimulationHandler.h"
#include <brayns/parameters/AnimationParameters.h>

namespace
{
const float DEFAULT_REST_VALUE = -80.f;
const float DEFAULT_SPIKING_VALUE = -1.f;
const float DEFAULT_TIME_INTERVAL = 0.01f;
const float DEFAULT_DECAY_SPEED = 1.f;
} // namespace

SpikeSimulationHandler::SpikeSimulationHandler(const std::string& reportPath,
                                               const brion::GIDSet& gids,
                                               const float transitionTime)
    : brayns::AbstractSimulationHandler()
    , _reportPath(reportPath)
    , _gids(gids)
    , _transition(transitionTime)
    , _spikeReport(new brain::SpikeReportReader(brain::URI(reportPath), gids))
{
    uint64_t c{0};
    for (const auto gid : _gids)
    {
        _gidMap[gid] = c;
        ++c;
    }

    // Load simulation information from compartment reports
    _nbFrames = _spikeReport->getEndTime() / DEFAULT_TIME_INTERVAL;
    _dt = DEFAULT_TIME_INTERVAL;
    _frameSize = _gids.size();
    _frameData.resize(_frameSize, DEFAULT_REST_VALUE);

    PLUGIN_INFO << "-----------------------------------------------------------"
                << std::endl;
    PLUGIN_INFO << "Spike simulation information" << std::endl;
    PLUGIN_INFO << "----------------------" << std::endl;
    PLUGIN_INFO << "Report path           : " << _reportPath << std::endl;
    PLUGIN_INFO << "Frame size (# of GIDs): " << _frameSize << std::endl;
    PLUGIN_INFO << "End time              : " << _spikeReport->getEndTime()
                << std::endl;
    PLUGIN_INFO << "Time interval         : " << DEFAULT_TIME_INTERVAL
                << std::endl;
    PLUGIN_INFO << "Transition time       : " << _transition
                << std::endl;
    PLUGIN_INFO << "Number of frames      : " << _nbFrames << std::endl;
    PLUGIN_INFO << "-----------------------------------------------------------"
                << std::endl;
}

SpikeSimulationHandler::SpikeSimulationHandler(
    const SpikeSimulationHandler& rhs)
    : brayns::AbstractSimulationHandler(rhs)
    , _reportPath(rhs._reportPath)
    , _gids(rhs._gids)
    , _spikeReport(rhs._spikeReport)
    , _gidMap(rhs._gidMap)
{
}

void* SpikeSimulationHandler::getFrameData(const uint32_t frame)
{
    const auto boundedFrame = _getBoundedFrame(frame);
    if (_currentFrame != boundedFrame)
    {
        std::fill(_frameData.begin(), _frameData.end(), DEFAULT_REST_VALUE);

        const float currentFrameTime = static_cast<float>(boundedFrame) * _dt;
        const auto trStart = currentFrameTime - _transition;
        const auto trEnd = currentFrameTime + _transition;
        const auto spikes = _spikeReport->getSpikes(trStart, trEnd);
        for (const auto& spike : spikes)
        {
            const auto gid = _gidMap[spike.second];
            const auto spikeTime = spike.first;


            // Spike in the future - start growth
            if(spikeTime > currentFrameTime)
            {
                auto alpha = (spikeTime - currentFrameTime) / _transition;
                alpha = std::min(std::max(0.f, alpha), 1.f);
                _frameData[gid] = DEFAULT_REST_VALUE * alpha +
                                  DEFAULT_SPIKING_VALUE * (1.f - alpha);
            }
            // Spike in the past - start fading
            else if(spikeTime < currentFrameTime)
            {
                auto alpha = (currentFrameTime - spikeTime) / _transition;
                alpha = std::min(std::max(0.f, alpha), 1.f);
                _frameData[gid] = DEFAULT_REST_VALUE * alpha +
                                  DEFAULT_SPIKING_VALUE * (1.f - alpha);
            }
            // Spiking neuron
            else
                _frameData[_gidMap[gid]] = DEFAULT_SPIKING_VALUE;
        }
        _currentFrame = boundedFrame;
    }

    return _frameData.data();
}

brayns::AbstractSimulationHandlerPtr SpikeSimulationHandler::clone() const
{
    return std::make_shared<SpikeSimulationHandler>(*this);
}
