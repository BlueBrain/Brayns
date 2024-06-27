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
using namespace brayns::experimental;

Data toSharedData3D(const void *data, VoxelDataType voxelDataType, const Size3 &size)
{
    auto type = static_cast<OSPDataType>(voxelDataType);
    auto handle = ospNewSharedData(data, type, size[0], 0, size[1], 0, size[2]);
    return Data(handle);
}
}

namespace brayns::experimental
{
OSPVolume ObjectReflector<RegularVolume>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewVolume("structuredRegular");
    throwLastDeviceErrorIfNull(device, handle);

    auto data = toSharedData3D(settings.data, settings.voxelDataType, settings.size);

    setObjectParam(handle, "data", data);
    setObjectParam(handle, "cellCentered", settings.cellCentered);
    setObjectParam(handle, "filter", static_cast<OSPVolumeFilter>(settings.filter));
    setObjectParam(handle, "background", settings.background);

    commitObject(handle);

    return handle;
}
}
