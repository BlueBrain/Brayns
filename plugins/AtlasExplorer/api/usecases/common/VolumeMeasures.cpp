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

#include "VolumeMeasures.h"

VolumeMeasures VolumeMeasuresComputer::compute(const NRRDHeader &header, size_t headerDataOffset)
{
    auto &sizes = header.sizes;
    assert(sizes.size() - headerDataOffset <= 3);

    auto &spacings = header.spacings;

    VolumeMeasures result;
    auto &volumeSizes = result.sizes;
    auto &volumeDimensions = result.dimensions;
    for (size_t i = headerDataOffset; i < sizes.size(); ++i)
    {
        volumeSizes[i - headerDataOffset] = sizes[i];
        if (spacings)
        {
            volumeDimensions[i - headerDataOffset] = (*spacings)[i];
        }
    }

    return result;
}
