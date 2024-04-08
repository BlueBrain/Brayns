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

#include "SpikeUtils.h"

#include <brayns/core/utils/MathTypes.h>

namespace
{
struct SpikeConstants
{
    static inline constexpr float restValue = 0.f;
    static inline constexpr float excitedValue = 1.f;
};
}

SpikeCalculator::SpikeCalculator(float interval):
    _invInterval(interval > 0.f ? 1.f / interval : 0.f)
{
}

float SpikeCalculator::compute(float spikeTime, float currentTime) const noexcept
{
    // Spike in the future - start growth
    if (spikeTime > currentTime)
    {
        auto alpha = brayns::math::clamp((spikeTime - currentTime) * _invInterval, 0.f, 1.f);
        return SpikeConstants::restValue * alpha + SpikeConstants::excitedValue * (1.0f - alpha);
    }

    // Spike in the past - start fading
    if (spikeTime < currentTime)
    {
        auto alpha = brayns::math::clamp((currentTime - spikeTime) * _invInterval, 0.f, 1.f);
        return SpikeConstants::restValue * alpha + SpikeConstants::excitedValue * (1.0f - alpha);
    }

    // Spiking neuron
    return SpikeConstants::excitedValue;
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
