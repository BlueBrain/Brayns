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

namespace brayns
{
RegularVolume createRegularVolume(Device &device, const RegularVolumeSettings &settings)
{
    auto handle = ospNewVolume("structuredRegular");
    auto volume = wrapObjectHandleAs<RegularVolume>(device, handle);

    auto type = static_cast<OSPDataType>(settings.voxelDataType);
    auto [x, y, z] = settings.size;
    auto dataHandle = ospNewSharedData(settings.data, type, x, 0, y, 0, z);
    auto data = Data(dataHandle);

    setObjectParam(handle, "data", data);
    setObjectParam(handle, "cellCentered", settings.cellCentered);
    setObjectParam(handle, "filter", static_cast<OSPVolumeFilter>(settings.filter));
    setObjectParam(handle, "background", settings.background);

    commitObject(handle);

    return volume;
}
}
