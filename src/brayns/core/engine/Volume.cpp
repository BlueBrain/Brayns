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

#include "Volume.h"

namespace
{
using namespace brayns;

void setRegularVolumeParams(OSPVolume handle, const RegularVolumeSettings &settings)
{
    setObjectParam(handle, "gridOrigin", settings.origin);
    setObjectParam(handle, "gridSpacing", settings.spacing);
    setObjectParam(handle, "cellCentered", settings.cellCentered);
    setObjectParam(handle, "filter", static_cast<OSPVolumeFilter>(settings.filter));
    setObjectParam(handle, "background", settings.background);
}
}

namespace brayns
{
std::size_t getSize(VoxelType type)
{
    switch (type)
    {
    case VoxelType::U8:
        return 1;
    case VoxelType::U16:
        return 2;
    case VoxelType::F32:
        return 4;
    case VoxelType::F64:
        return 8;
    default:
        throw std::invalid_argument("Invalid volume data type");
    }
}

void RegularVolume::update(const RegularVolumeSettings &settings)
{
    auto handle = getHandle();
    setRegularVolumeParams(handle, settings);
    commitObject(handle);
}

RegularVolume createRegularVolume(Device &device, const RegularVolumeData &data, const RegularVolumeSettings &settings)
{
    auto handle = ospNewVolume("structuredRegular");
    auto volume = wrapObjectHandleAs<RegularVolume>(device, handle);

    auto wrapper = createData(data.value.data(), static_cast<DataType>(data.type), data.size);

    setObjectParam(handle, "data", wrapper);

    setRegularVolumeParams(handle, settings);

    commitObject(device, handle);

    return volume;
}
}
