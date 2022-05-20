/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#include "HeaderLimitCheck.h"

namespace
{
class DimensionLimitsChecker
{
public:
    static void check(const NRRDHeader &header)
    {
        const auto dimensions = header.dimensions;
        if (dimensions > 4)
        {
            throw std::runtime_error("Only up to 4 dimensional volumes are supported");
        }

        auto &kinds = header.kinds;
        auto &sizes = header.sizes;
        if (!kinds && dimensions == 4 && sizes[0] > 1)
        {
            throw std::runtime_error("Cannot interpret a non-scalar fourth dimension without kind information");
        }
        if (kinds && (*kinds)[0] == NRRDKind::NONE && dimensions == 4 && sizes[0] > 1)
        {
            throw std::runtime_error("Cannot interpret a non-scalar fourth dimension with kind 'none'");
        }
        if (dimensions == 4 && sizes[0] > 4)
        {
            throw std::runtime_error("Cannot represent voxel data of more than 4 dimenssions");
        }
    }
};

class SingleRangeKindChecker
{
public:
    static void check(const NRRDHeader &header)
    {
        const auto &kinds = header.kinds;
        if (!kinds)
        {
            return;
        }

        const auto dimensions = static_cast<size_t>(header.dimensions);

        std::optional<size_t> rangeIndex;
        for (size_t i = 0; i < dimensions; ++i)
        {
            auto kind = (*kinds)[i];
            if (kind == NRRDKind::DOMAIN || kind == NRRDKind::SPACE)
            {
                continue;
            }
            if (rangeIndex)
            {
                throw std::runtime_error("Only 1 'range' kind supported");
            }
            rangeIndex = i;
        }
    }
};
}

void HeaderLimitCheck::check(const NRRDHeader &header)
{
    DimensionLimitsChecker::check(header);
    SingleRangeKindChecker::check(header);
}
