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

void HeaderLimitCheck::check(const NRRDHeader &header)
{
    const auto dimensions = header.dimensions;
    if (dimensions != 3 && dimensions != 4)
    {
        throw std::runtime_error("Only 3D scalar and 4D orientation volumes are supported");
    }

    const auto &sizes = header.sizes;
    if (dimensions == 4 && sizes[0] != 4)
    {
        throw std::runtime_error("Only 4D volumes suported are orientation fields");
    }

    const auto &spaceDimensions = header.spaceDimensions;
    if (spaceDimensions && *spaceDimensions != 3)
    {
        throw std::runtime_error("Only 3D spatial volumes are allowed (space dimensions must be 3)");
    }
}
