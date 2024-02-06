/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/utils/MathTypes.h>

namespace bbploader
{
SpikeData::SpikeData(
    std::unique_ptr<brain::SpikeReportReader> report,
    const std::vector<uint64_t> &gids,
    float spikeInterval):
    _report(std::move(report)),
    _spikeCalculator(spikeInterval),
    _interval(spikeInterval),
    _mapping(SpikeMappingGenerator::generate(gids))
{
}

double SpikeData::getStartTime() const noexcept
{
    return 0.;
}

double SpikeData::getEndTime() const noexcept
{
    return _report->getEndTime();
}

double SpikeData::getTimeStep() const noexcept
{
    return 0.01;
}

std::string SpikeData::getTimeUnit() const noexcept
{
    return "";
}

std::vector<float> SpikeData::getFrame(double timestamp) const
{
    auto fTimestamp = static_cast<float>(brayns::math::clamp(timestamp, 0., getEndTime()));

    auto limitTimestamp = _report->getEndTime() - 0.01f;
    auto frameStart = brayns::math::clamp(fTimestamp - _interval, 0.f, limitTimestamp);
    auto frameEnd = brayns::math::clamp(fTimestamp + _interval, 0.f, limitTimestamp);

    auto spikes = _report->getSpikes(frameStart, frameEnd);

    auto values = std::vector<float>(_mapping.size(), 0.f);

    for (size_t i = 0; i < spikes.size(); ++i)
    {
        auto &spike = spikes[i];
        auto gid = spike.second;

        auto it = _mapping.find(gid);
        if (it == _mapping.end())
        {
            continue;
        }

        auto index = it->second;
        auto spikeTime = spike.first;

        values[index] = _spikeCalculator.compute(spikeTime, fTimestamp);
    }

    return values;
}
}
