/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Authors: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *          Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "SpikeHandler.h"

namespace bbploader
{
namespace
{
constexpr float DEFAULT_REST_VALUE = -80.f;
constexpr float DEFAULT_SPIKING_VALUE = -1.f;
constexpr float DEFAULT_TIME_INTERVAL = 0.01f;

auto frameIndexToTimestamp(const uint32_t frame, const double dt) noexcept
{
    return static_cast<double>(std::nextafter(dt, INFINITY) * frame);
}
} // namespace

SpikeHandler::SpikeHandler(
    const std::string& path, const float transitionTime,
    const brain::GIDSet& gids,
    const std::shared_ptr<brain::SpikeReportReader>& report)
    : brayns::AbstractSimulationHandler()
    , _path(path)
    , _transition(transitionTime)
    , _report(report)
{
    uint64_t c{0};
    for (const auto gid : gids)
    {
        _gidMap[gid] = c;
        ++c;
    }

    // Load simulation information from compartment reports
    _startTime = 0.0;
    _endTime = _report->getEndTime();
    _nbFrames = _endTime / DEFAULT_TIME_INTERVAL;
    _dt = DEFAULT_TIME_INTERVAL;
    _frameSize = gids.size();
}

SpikeHandler::SpikeHandler(const SpikeHandler& other)
    : brayns::AbstractSimulationHandler(other)
    , _path(other._path)
    , _transition(other._transition)
    , _report(other._report)
    , _gidMap(other._gidMap)
    , _ready(false)
{
}

brayns::AbstractSimulationHandlerPtr SpikeHandler::clone() const
{
    return std::make_shared<SpikeHandler>(*this);
}

std::vector<float> SpikeHandler::getFrameDataImpl(const uint32_t frame)
{
    _ready = false;
    std::vector<float> data(_frameSize, DEFAULT_REST_VALUE);

    const auto timestamp = frameIndexToTimestamp(frame, _dt);

    const auto trStart = timestamp - _transition;
    const auto trEnd = timestamp + _transition;
    const auto spikes = _report->getSpikes(trStart, trEnd);
    for (const auto& spike : spikes)
    {
        const auto index = _gidMap[spike.second];
        const auto spikeTime = spike.first;

        // Spike in the future - start growth
        if (spikeTime > timestamp)
        {
            auto alpha = (spikeTime - timestamp) / _transition;
            alpha = std::min(std::max(0.0, alpha), 1.0);
            data[index] = DEFAULT_REST_VALUE * alpha +
                          DEFAULT_SPIKING_VALUE * (1.0 - alpha);
        }
        // Spike in the past - start fading
        else if (spikeTime < timestamp)
        {
            auto alpha = (timestamp - spikeTime) / _transition;
            alpha = std::min(std::max(0.0, alpha), 1.0);
            data[index] = DEFAULT_REST_VALUE * alpha +
                          DEFAULT_SPIKING_VALUE * (1.0 - alpha);
        }
        // Spiking neuron
        else
            data[index] = DEFAULT_SPIKING_VALUE;
    }
    _ready = true;
    return data;
}

bool SpikeHandler::isReady() const
{
    return _ready;
}
} // namespace bbploader
