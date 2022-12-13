/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "SonataSpikeData.h"

#include <api/reports/common/FrameTimeCalculator.h>
#include <api/reports/common/SpikeUtils.h>

namespace sonataloader
{
SonataSpikeData::SonataSpikeData(
    const std::string &reportPath,
    const std::string &population,
    bbp::sonata::Selection selection,
    float interval)
    : _reader(bbp::sonata::SpikeReader(reportPath))
    , _population(_reader.openPopulation(population))
    , _selection(std::move(selection))
    , _mapping(SpikeMappingGenerator::generate(_selection.flatten()))
    , _calculator(interval)
    , _interval(interval)
{
    auto [start, end] = _population.getTimes();
    _start = static_cast<float>(start);
    _end = static_cast<float>(end);
}

float SonataSpikeData::getStartTime() const noexcept
{
    return _start;
}

float SonataSpikeData::getEndTime() const noexcept
{
    return _end;
}

float SonataSpikeData::getTimeStep() const noexcept
{
    return 0.01f;
}

std::string SonataSpikeData::getTimeUnit() const noexcept
{
    return "";
}

std::vector<float> SonataSpikeData::getFrame(uint32_t frameIndex) const
{
    auto data = std::vector<float>(_mapping.size(), 0.f);

    auto start = getStartTime();
    auto end = getEndTime();
    auto dt = getTimeStep();
    auto frameTime = FrameTimeCalculator::compute(frameIndex, start, end, dt);
    auto frameStart = frameTime - _interval;
    auto frameEnd = frameTime + _interval;

    auto spikes = _population.get(_selection, frameStart, frameEnd);

    for (size_t i = 0; i < spikes.size(); ++i)
    {
        auto &spike = spikes[i];
        auto nodeId = spike.first;
        auto spikeTime = static_cast<float>(spike.second);
        auto it = _mapping.find(nodeId);

        if (it == _mapping.end())
        {
            continue;
        }

        auto index = it->second;
        data[index] = _calculator.compute(spikeTime, frameTime);
    }

    return data;
}
}
