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

#include "SpikeUtils.h"

namespace
{
constexpr float SPIKE_REST_VALUE = 0.f;
constexpr float SPIKE_EXCITED_VALUE = 1.f;
}

SpikeCalculator::SpikeCalculator(const float interval)
    : _invInterval(interval > 0.f ? 1.f / interval : 0.f)
{
}

float SpikeCalculator::compute(float spikeTime, float currentTime) const noexcept
{
    float value{};

    // Spike in the future - start growth
    if (spikeTime > currentTime)
    {
        auto alpha = (spikeTime - currentTime) * _invInterval;
        alpha = alpha < 0.f ? 0.f : (alpha > 1.f ? 1.f : alpha);
        value = SPIKE_REST_VALUE * alpha + SPIKE_EXCITED_VALUE * (1.0 - alpha);
    }
    // Spike in the past - start fading
    else if (spikeTime < currentTime)
    {
        auto alpha = (currentTime - spikeTime) * _invInterval;
        alpha = alpha < 0.f ? 0.f : (alpha > 1.f ? 1.f : alpha);
        value = SPIKE_REST_VALUE * alpha + SPIKE_EXCITED_VALUE * (1.0 - alpha);
    }
    // Spiking neuron
    else
    {
        value = SPIKE_EXCITED_VALUE;
    }

    return value;
}

std::unordered_map<uint64_t, size_t> SpikeMappingGenerator::generate(const std::vector<uint64_t> &ids) noexcept
{
    std::unordered_map<uint64_t, size_t> result;
    for (size_t i = 0; i < ids.size(); ++i)
    {
        const auto id = ids[i];
        result[id] = i;
    }
    return result;
}
