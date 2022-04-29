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
    const auto [start, end] = _population.getTimes();
    _start = start;
    _end = end;
}

size_t SonataSpikeData::getFrameSize() const noexcept
{
    return _mapping.size();
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

std::vector<float> SonataSpikeData::getFrame(const uint32_t frameIndex) const
{
    std::vector<float> data(getFrameSize(), 0.f);

    const auto start = getStartTime();
    const auto end = getEndTime();
    const auto dt = getTimeStep();
    const auto frameTime = FrameTimeCalculator::compute(frameIndex, start, end, dt);
    const auto frameStart = frameTime - _interval;
    const auto frameEnd = frameTime + _interval;

    const auto spikes = _population.get(
        nonstd::optional<bbp::sonata::Selection>(_selection),
        nonstd::optional<double>(frameStart),
        nonstd::optional<double>(frameEnd));

    for (size_t i = 0; i < spikes.size(); ++i)
    {
        const auto &spike = spikes[i];
        const auto gid = spike.first;
        const auto spikeTime = spike.second;
        const auto index = _mapping.at(gid);

        data[index] = _calculator.compute(spikeTime, frameTime);
    }

    return data;
}
}
