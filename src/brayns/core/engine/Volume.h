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

#include <optional>
#include <string>

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

enum class VoxelType
{
    U8 = OSP_UCHAR,
    U16 = OSP_USHORT,
    F32 = OSP_FLOAT,
    F64 = OSP_DOUBLE,
};

std::size_t getSize(VoxelType type);

struct RegularVolumeData
{
    std::vector<char> value;
    VoxelType type;
    Size3 size;
};

struct RegularVolumeSettings
{
    Vector3 origin = {0.0F, 0.0F, 0.0F};
    Vector3 spacing = {1.0F, 1.0F, 1.0F};
    bool cellCentered = false;
    VolumeFilter filter = VolumeFilter::Linear;
    std::optional<float> background = {};
};

class RegularVolume : public Volume
{
public:
    using Volume::Volume;

    void update(const RegularVolumeSettings &settings);
};

RegularVolume createRegularVolume(Device &device, const RegularVolumeData &data, const RegularVolumeSettings &settings = {});
}
