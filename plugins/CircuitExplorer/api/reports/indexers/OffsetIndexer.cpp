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

#include "OffsetIndexer.h"

OffsetIndexer::OffsetIndexer(std::vector<size_t> offsets)
    : _offsets(std::move(offsets))
{
}

std::vector<uint8_t> OffsetIndexer::generate(const std::vector<float> &data, const brayns::Vector2f &range) noexcept
{
    std::vector<uint8_t> indices(_offsets.size());

    const auto rangeStart = range.x;
    const auto rangeEnd = range.y;
    const auto invFactor = 1.f / std::fabs(rangeEnd - rangeStart);

    for (size_t i = 0; i < _offsets.size(); ++i)
    {
        const auto offset = _offsets[i];
        auto value = data[offset];
        value = value > rangeEnd ? rangeEnd : (value < rangeStart ? rangeStart : value);

        const auto normIndex = (value - rangeStart) * invFactor;
        indices[i] = static_cast<uint8_t>(normIndex * 255.f);
    }

    return indices;
}
