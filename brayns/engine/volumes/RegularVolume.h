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
#include <brayns/engine/Volume.h>
#include <brayns/engine/VolumeDataType.h>

#include <vector>

namespace brayns
{
/**
 * @brief The RegularVolume struct is a regular grid volume in which the data is laid out in XYZ order:
 * The first elements are the X values of the first row of the first frame.
 */
struct RegularVolume
{
    // Specifies how to interpret the bytes stored as data
    VolumeDataType dataType;
    std::vector<uint8_t> data;
    Vector3ui size{0u};
    // Specifies wether the data is scpeified as per grid vertex. If false, is specified as per grid cell center.
    bool perVertexData{true};
};

template<>
class VolumeOSPRayID<RegularVolume>
{
public:
    static std::string_view get();
};

template<>
class VolumeBoundsUpdater<RegularVolume>
{
public:
    static void update(const RegularVolume &s, const Matrix4f &t, Bounds &b);
};

template<>
class VolumeCommitter<RegularVolume>
{
public:
    static void commit(OSPVolume handle, const RegularVolume &volumeData);
};
}
