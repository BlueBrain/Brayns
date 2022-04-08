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

#include "SpikeData.h"

#include <api/reports/common/FrameTimeCalculator.h>

namespace bbploader
{
SpikeData::SpikeData(
    std::unique_ptr<brain::SpikeReportReader> report,
    const std::vector<uint64_t> &gids,
    float spikeInterval)
    : _report(std::move(report))
    , _spikeCalculator(spikeInterval)
    , _interval(spikeInterval)
    , _mapping(SpikeMappingGenerator::generate(gids))
{
}

size_t SpikeData::getFrameSize() const noexcept
{
    return _mapping.size();
}

float SpikeData::getStartTime() const noexcept
{
    return 0.f;
}

float SpikeData::getEndTime() const noexcept
{
    return _report->getEndTime();
}

float SpikeData::getTimeStep() const noexcept
{
    return 0.01f;
}

std::string SpikeData::getTimeUnit() const noexcept
{
    return "";
}

std::vector<float> SpikeData::getFrame(const uint32_t frameIndex) const
{
    std::vector<float> values(_mapping.size(), 0.f);

    const auto start = getStartTime();
    const auto end = getEndTime();
    const auto dt = getTimeStep();
    const auto frameTime = FrameTimeCalculator::compute(frameIndex, start, end, dt);
    const auto frameStart = frameTime - _interval;
    const auto frameEnd = frameTime + _interval;

    const auto spikes = _report->getSpikes(frameStart, frameEnd);

    // #pragma omp parallel for
    for (size_t i = 0; i < spikes.size(); ++i)
    {
        const auto &spike = spikes[i];
        const auto gid = spike.second;
        const auto index = _mapping.at(gid);
        const auto spikeTime = spike.first;

        values[index] = _spikeCalculator.compute(spikeTime, frameTime);
    }

    return values;
}
}
