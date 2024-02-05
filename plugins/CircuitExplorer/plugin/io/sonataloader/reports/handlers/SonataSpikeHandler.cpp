/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include "SonataSpikeHandler.h"

namespace sonataloader
{
namespace
{
constexpr float SPIKE_HANDLER_DT = 0.1f;
constexpr float SPIKE_DEFAULT_REST_VALUE = -80.f;
constexpr float SPIKE_DEFAULT_SPIKING_VALUE = -1.f;
constexpr float SPIKE_TRANSITION_TIME_SECONDS = 0.05f;

auto frameIndexToTimestamp(const uint32_t frame, const double dt) noexcept
{
    return static_cast<double>(std::nextafter(dt, INFINITY) * frame);
}
} // namespace

SonataSpikeHandler::SonataSpikeHandler(const std::string& h5FilePath,
                                       const std::string& populationName,
                                       const bbp::sonata::Selection& selection)
    : brayns::AbstractSimulationHandler()
    , _h5FilePath(h5FilePath)
    , _populationName(populationName)
    , _selection(selection)
    , _reader(bbp::sonata::SpikeReader(h5FilePath))
    , _spikePopulation(_reader.openPopulation(populationName))
{
    const auto allSpikes = _spikePopulation.get(selection);

    // Temporary fix until bbp::sonata::SpikeReader returns the start and end
    // time
    double greatestTime = 0.0;
    for (const auto& spike : allSpikes)
    {
        if (spike.second > greatestTime)
            greatestTime = spike.second;
    }
    // If the selection does not contain any spiking cell, greatestTime will be
    // 0 and the simulation rendering will not be triggered (even though it does
    // not have anything to show). We still want to be able to render this as
    // the spike report allow to load non spiking cells which will remain in
    // resting mode through out the simulation
    if (greatestTime == 0.0)
        greatestTime = 1.0;

    _startTime = 0.0;
    _endTime = greatestTime + SPIKE_HANDLER_DT;
    _dt = SPIKE_HANDLER_DT;
    _unit = "";

    _frameSize = selection.flatSize();
    _nbFrames = static_cast<uint32_t>(
        std::round(std::nextafter(_endTime, INFINITY) / _dt));

    const auto flatSelection = _selection.flatten();
    for (size_t i = 0; i < flatSelection.size(); ++i)
        _gidsToIndex[flatSelection[i]] = i;
}

brayns::AbstractSimulationHandlerPtr SonataSpikeHandler::clone() const
{
    return std::make_shared<SonataSpikeHandler>(_h5FilePath, _populationName,
                                                _selection);
}

std::vector<float> SonataSpikeHandler::getFrameDataImpl(const uint32_t frame)
{
    _ready = false;
    // const auto realFrame = frame > _nbFrames? _nbFrames : frame;
    const auto timestamp = frameIndexToTimestamp(frame, _dt);
    std::vector<float> data(_frameSize, SPIKE_DEFAULT_REST_VALUE);

    const auto trStart =
        std::max(timestamp - SPIKE_TRANSITION_TIME_SECONDS, 0.0);
    const auto trEnd =
        std::min(timestamp + SPIKE_TRANSITION_TIME_SECONDS, _endTime);
    const auto readSpikes = _spikePopulation.get(_selection, trStart, trEnd);
    for (const auto& spike : readSpikes)
    {
        const auto index = _gidsToIndex[spike.first];
        const auto spikeTime = spike.second;

        // Spike in the future - start growth
        if (spikeTime > timestamp)
        {
            auto alpha =
                (spikeTime - timestamp) / SPIKE_TRANSITION_TIME_SECONDS;
            alpha = std::min(std::max(0.0, alpha), 1.0);
            data[index] = SPIKE_DEFAULT_REST_VALUE * alpha +
                          SPIKE_DEFAULT_SPIKING_VALUE * (1.0 - alpha);
        }
        // Spike in the past - start fading
        else if (spikeTime < timestamp)
        {
            auto alpha =
                (timestamp - spikeTime) / SPIKE_TRANSITION_TIME_SECONDS;
            alpha = std::min(std::max(0.0, alpha), 1.0);
            data[index] = SPIKE_DEFAULT_REST_VALUE * alpha +
                          SPIKE_DEFAULT_SPIKING_VALUE * (1.0 - alpha);
        }
        // Spiking neuron
        else
            data[index] = SPIKE_DEFAULT_SPIKING_VALUE;
    }

    _ready = true;

    return data;
}
} // namespace sonataloader
