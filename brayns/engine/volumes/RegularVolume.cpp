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

#include "RegularVolume.h"

#include <brayns/engine/common/DataHandler.h>

namespace brayns
{
size_t getTypeByteSize(const OSPDataType type)
{
    switch (type)
    {
    case OSPDataType::OSP_CHAR:
    case OSPDataType::OSP_UCHAR:
        return 1;
    case OSPDataType::OSP_SHORT:
    case OSPDataType::OSP_USHORT:
    case OSPDataType::OSP_HALF:
        return 2;
    case OSPDataType::OSP_INT:
    case OSPDataType::OSP_UINT:
    case OSPDataType::OSP_FLOAT:
        return 4;
    case OSPDataType::OSP_LONG:
    case OSPDataType::OSP_ULONG:
    case OSPDataType::OSP_DOUBLE:
        return 8;
    default:
        throw std::runtime_error("Unsupported volume data type");
        return 0;
    }
}
}

namespace brayns
{
std::string_view VolumeOSPRayID<RegularVolume>::get()
{
    return "structuredRegular";
}

void VolumeBoundsUpdater<RegularVolume>::update(const RegularVolume &s, const Matrix4f &t, Bounds &b)
{
    static const Vector3f regularVolumeMin{0.f};
    static const Vector3f regularVolumeMax{1.f};

    const Vector3f minBound(t * Vector4f(regularVolumeMin, 1.f));
    const Vector3f maxBound(t * Vector4f(regularVolumeMax, 1.f));

    b.expand(minBound);
    b.expand(maxBound);
}

void VolumeCommitter<RegularVolume>::commit(OSPVolume handle, const RegularVolume &volumeData)
{
    const auto cellCentered = !volumeData.perVertexData;
    const auto dataType = static_cast<OSPDataType>(volumeData.dataType);
    const auto &data = volumeData.data;
    const auto &size = volumeData.size;

    const auto dimensionSize = glm::compMul(size);
    if (dimensionSize == 0)
    {
        throw std::runtime_error("Tried to commit volume with 0 size");
    }

    const auto currentSize = data.size();
    const auto expectedSize = getTypeByteSize(dataType) * dimensionSize;
    if (currentSize != expectedSize)
    {
        throw std::runtime_error("RegularVolume expected size and current size missmatch");
    }

    auto buffer = DataHandler::shareBuffer(data, dataType);
    ospSetParam(handle, "data", OSPDataType::OSP_DATA, &buffer.handle);
    ospSetParam(handle, "cellCentered", OSPDataType::OSP_BOOL, &cellCentered);
}
}
