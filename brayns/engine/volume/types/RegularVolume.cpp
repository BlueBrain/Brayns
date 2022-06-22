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

#include <brayns/engine/common/MathTypesOsprayTraits.h>

#include <ospray/ospray.h>

namespace
{
class RegularVolumeIntegrityChecker
{
public:
    static void check(const brayns::RegularVolume &volumeData)
    {
        const auto dataType = static_cast<OSPDataType>(volumeData.dataType);
        const auto &data = volumeData.data;
        const auto &size = volumeData.size;
        const auto dimensionSize = glm::compMul(size);
        if (dimensionSize == 0)
        {
            throw std::runtime_error("Tried to commit volume with 0 size");
        }

        const auto currentSize = data.size();
        const auto expectedSize = _getTypeByteSize(dataType) * dimensionSize;
        if (currentSize != expectedSize)
        {
            throw std::runtime_error("RegularVolume expected size and current size missmatch");
        }
    }

private:
    static size_t _getTypeByteSize(const OSPDataType type)
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
};

struct RegularVolumeParameters
{
    inline static const std::string osprayName = "structuredRegular";
    inline static const std::string data = "data";
    inline static const std::string cellCentered = "cellCentered";
    inline static const std::string gridSpacing = "gridSpacing";
};
}

namespace brayns
{
const std::string &OsprayVolumeName<RegularVolume>::get()
{
    return RegularVolumeParameters::osprayName;
}

void VolumeBoundsUpdater<RegularVolume>::update(const RegularVolume &s, const Matrix4f &t, Bounds &b)
{
    const auto size = brayns::Vector3f(s.size) * s.spacing;
    const Vector3f minBound(t * Vector4f(0.f, 0.f, 0.f, 1.f));
    const Vector3f maxBound(t * Vector4f(size, 1.f));

    b.expand(minBound);
    b.expand(maxBound);
}

void VolumeCommitter<RegularVolume>::commit(const ospray::cpp::Volume &osprayVolume, const RegularVolume &volumeData)
{
    const auto cellCentered = !volumeData.perVertexData;
    const auto dataType = static_cast<OSPDataType>(volumeData.dataType);
    const auto &data = volumeData.data;
    const auto &size = volumeData.size;
    const auto &spacing = volumeData.spacing;

    OSPData sharedData = ospNewSharedData(data.data(), dataType, size.x, 0, size.y, 0, size.z);
    osprayVolume.setParam(RegularVolumeParameters::data, sharedData);
    ospRelease(sharedData);

    osprayVolume.setParam(RegularVolumeParameters::cellCentered, cellCentered);
    osprayVolume.setParam(RegularVolumeParameters::gridSpacing, spacing);
}
}
