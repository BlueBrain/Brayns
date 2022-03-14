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
#include <brayns/engine/RenderableType.h>

#include <vector>

#include <ospray/ospray.h>

namespace brayns
{
enum class RegularVolumeDataType: uint32_t
{
    BYTE = OSPDataType::OSP_UCHAR,
    SHORT = OSPDataType::OSP_SHORT,
    UNSIGNED_SHORT = OSPDataType::OSP_USHORT,
    HALF_FLOAT = OSPDataType::OSP_HALF,
    FLOAT = OSPDataType::OSP_FLOAT,
    DOUBLE = OSPDataType::OSP_DOUBLE
};

/**
 * @brief The RegularVolume struct is a regular grid volume in which the data is laid out in XYZ order:
 * The first elements are the X values of the first row of the first frame.
 */
struct RegularVolume
{
    // Specifies how to interpret the bytes stored as data
    RegularVolumeDataType dataType;
    std::vector<uint8_t> data;
    Vector3ui size {0u};
    // Specifies wether the data is scpeified as per grid vertex. If false, is specified as per grid cell center.
    bool perVertexData {true};
};

template<>
std::string_view RenderableOSPRayID<RegularVolume>::get();

template<>
void RenderableBoundsUpdater<RegularVolume>::update(const RegularVolume& s, const Matrix4f& t, Bounds& b);

template<>
uint64_t Volume<RegularVolume>::getSizeInBytes() const noexcept;

template<>
void Volume<RegularVolume>::commitVolumeSpecificParams();

}
