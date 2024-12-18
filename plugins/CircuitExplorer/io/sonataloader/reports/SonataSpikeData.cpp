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

#include "SonataSpikeData.h"

#include <ranges>
#include <unordered_set>

#include <brayns/utils/MathTypes.h>

namespace sonataloader
{
SonataSpikeData::SonataSpikeData(
    const std::string &reportPath,
    const std::string &population,
    bbp::sonata::Selection selection,
    float interval):
    _reader(bbp::sonata::SpikeReader(reportPath)),
    _population(_reader.openPopulation(population)),
    _selection(std::move(selection)),
    _mapping(SpikeMappingGenerator::generate(_selection.flatten())),
    _calculator(interval),
    _interval(interval)
{
    auto [start, end] = _population.getTimes();
    _start = start;
    _end = end;
}

double SonataSpikeData::getStartTime() const noexcept
{
    return _start;
}

double SonataSpikeData::getEndTime() const noexcept
{
    return _end;
}

double SonataSpikeData::getTimeStep() const noexcept
{
    return 0.01;
}

std::string SonataSpikeData::getTimeUnit() const noexcept
{
    return "";
}

std::vector<float> SonataSpikeData::getFrame(double timestamp) const
{
    auto frameStart = brayns::math::clamp(timestamp - _interval, _start, _end);
    auto frameEnd = brayns::math::clamp(timestamp + _interval, _start, _end);

    auto spikes = _population.get({}, frameStart, frameEnd);

    auto values = _selection.flatten();
    auto selection = std::unordered_set<bbp::sonata::Selection::Value>(values.begin(), values.end());

    auto notSelected = [&](const auto &spike) { return !selection.contains(spike.first); };
    auto [first, last] = std::ranges::remove_if(spikes, notSelected);
    spikes.erase(first, last);

    auto data = std::vector<float>(_mapping.size(), 0.f);

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
        data[index] = _calculator.compute(spikeTime, static_cast<float>(timestamp));
    }

    return data;
}
}
