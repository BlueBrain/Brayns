/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <limits>

#include "Data.h"
#include "Object.h"

namespace brayns::experimental
{
class Volume : public Managed<OSPVolume>
{
public:
    using Managed::Managed;
};

enum class VoxelDataType
{
    UChar = OSP_UCHAR,
    Short = OSP_SHORT,
    UShort = OSP_USHORT,
    Half = OSP_HALF,
    Float = OSP_FLOAT,
    Double = OSP_DOUBLE,
};

enum class VolumeFilter
{
    Nearest = OSP_VOLUME_FILTER_NEAREST,
    Linear = OSP_VOLUME_FILTER_LINEAR,
    Cubic = OSP_VOLUME_FILTER_CUBIC,
};

struct RegularVolumeSettings
{
    const void *data;
    VoxelDataType voxelDataType;
    Size3 size;
    bool cellCentered = false;
    VolumeFilter filter = VolumeFilter::Linear;
    float background = std::numeric_limits<float>::quiet_NaN();
};

class RegularVolume : public Volume
{
public:
    using Volume::Volume;
};

template<>
struct ObjectReflector<RegularVolume>
{
    using Settings = RegularVolumeSettings;

    static inline const std::string name = "structuredRegular";

    static void loadParams(OSPVolume handle, const Settings &settings);
};
}
