/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/core/utils/EnumInfo.h>

#include <cstdint>

#include <ospray/ospray.h>

namespace brayns
{
enum class VolumeDataType : uint32_t
{
    UnsignedChar = OSPDataType::OSP_UCHAR,
    Short = OSPDataType::OSP_SHORT,
    UnsignedShort = OSPDataType::OSP_USHORT,
    HalfFloat = OSPDataType::OSP_HALF,
    Float = OSPDataType::OSP_FLOAT,
    Double = OSPDataType::OSP_DOUBLE
};

template<>
struct EnumReflector<VolumeDataType>
{
    static EnumMap<VolumeDataType> reflect()
    {
        return {
            {"unsinged_char", VolumeDataType::UnsignedChar},
            {"short", VolumeDataType::Short},
            {"unsigned_short", VolumeDataType::UnsignedShort},
            {"half_float", VolumeDataType::HalfFloat},
            {"float", VolumeDataType::Float},
            {"double", VolumeDataType::Double}};
    }
};
}
