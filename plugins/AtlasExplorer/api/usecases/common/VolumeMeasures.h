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

#pragma once

#include <brayns/common/MathTypes.h>

#include <api/NRRDHeader.h>

struct VolumeMeasures
{
    brayns::Vector3f sizes = brayns::Vector3f(1.f);
    brayns::Vector3f dimensions = brayns::Vector3f(1.f);
};

class VolumeMeasuresComputer
{
public:
    /**
     * @brief Compute the sizes (number of samples per axis) and dimensions (spacing between samples per axis)
     * for a NRRD volume given its header information.
     *
     * @param header the NRRD header
     * @param headerDataOffset offset to use when accessing header per-axis information
     * @return VolumeMeasures
     */
    static VolumeMeasures compute(const NRRDHeader &header, size_t headerDataOffset = 0);
};
