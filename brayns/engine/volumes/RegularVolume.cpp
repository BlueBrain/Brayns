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

#include <brayns/engine/volumes/RegularVolume.h>

namespace brayns
{
size_t getTypeByteSize(const OSPDataType type)
{
    switch(type)
    {
    case OSPDataType::OSP_UCHAR:
        return 1;
    case OSPDataType::OSP_SHORT:
    case OSPDataType::OSP_USHORT:
    case OSPDataType::OSP_HALF:
        return 2;
    case OSPDataType::OSP_FLOAT:
        return 4;
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
template<>
std::string_view RenderableOSPRayID<RegularVolume>::get()
{
    return "structuredRegular";
}

template<>
void RenderableBoundsUpdater<RegularVolume>::update(const RegularVolume& s, const Matrix4f& t, Bounds& b)
{
    static const Vector3f regularVolumeMin {0.f};
    static const Vector3f regularVolumeMax {1.f};

    const Vector3f minBound(t * Vector4f(regularVolumeMin, 1.f));
    const Vector3f maxBound(t * Vector4f(regularVolumeMax, 1.f));

    b.expand(minBound);
    b.expand(maxBound);
}

template<>
uint64_t Volume<RegularVolume>::getSizeInBytes() const noexcept
{
    return sizeof(*this) + _volumeData.data.size();
}

template<>
void Volume<RegularVolume>::commitVolumeSpecificParams()
{
    const auto cellCentered = !_volumeData.perVertexData;
    const auto dataType = static_cast<OSPDataType>(_volumeData.dataType);
    const auto& volumeData = _volumeData.data;
    const auto &size = _volumeData.size;

    const auto dimensionSize = glm::compMul(size);
    if(dimensionSize == 0)
        throw std::runtime_error("Tried to commit volume with 0 size");

    const auto currentSize = volumeData.size();
    const auto expectedSize = getTypeByteSize(dataType) * dimensionSize;
    if(currentSize != expectedSize)
        throw std::runtime_error("RegularVolume expected size and current size missmatch");


    OSPData sharedVolumeData = ospNewSharedData(volumeData.data(), dataType, size.x, 0, size.y, 0, size.z, 0);
    ospSetParam(_handle, "data", OSPDataType::OSP_DATA, &sharedVolumeData);
    ospRelease(sharedVolumeData);

    ospSetParam(_handle, "cellCentered", OSPDataType::OSP_BOOL, &cellCentered);
}
}
