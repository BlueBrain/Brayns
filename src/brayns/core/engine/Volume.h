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
#include "Device.h"
#include "Object.h"

namespace brayns
{
class Volume : public Managed<OSPVolume>
{
public:
    using Managed::Managed;
};

enum class VolumeFilter
{
    Nearest = OSP_VOLUME_FILTER_NEAREST,
    Linear = OSP_VOLUME_FILTER_LINEAR,
    Cubic = OSP_VOLUME_FILTER_CUBIC,
};

using VolumeData = std::variant<Data3D<std::uint8_t>, Data3D<std::uint16_t>, Data3D<float>, Data3D<double>>;

enum class VoxelType
{
    CellCentered,
    VertexCentered,
};

struct RegularVolumeSettings
{
    VolumeData data;
    VoxelType voxelType = VoxelType::VertexCentered;
    VolumeFilter filter = VolumeFilter::Linear;
    float background = std::numeric_limits<float>::quiet_NaN();
};

class RegularVolume : public Volume
{
public:
    using Volume::Volume;
};

RegularVolume createRegularVolume(Device &device, const RegularVolumeSettings &settings);
}
