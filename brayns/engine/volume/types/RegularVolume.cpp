/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
class RegularVolumeIntegrityChecker
{
public:
    static void check(const brayns::RegularVolume &volumeData)
    {
        const auto dataType = static_cast<OSPDataType>(volumeData.dataType);
        const auto &voxels = volumeData.voxels;
        const auto &size = volumeData.size;
        const auto dimensionSize = brayns::math::reduce_mul(size);
        if (dimensionSize == 0)
        {
            throw std::runtime_error("Tried to commit volume with 0 size");
        }

        const auto currentSize = voxels.size();
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
    static inline const std::string osprayName = "structuredRegular";
    static inline const std::string data = "data";
    static inline const std::string cellCentered = "cellCentered";
    static inline const std::string gridSpacing = "gridSpacing";
};
}

namespace brayns
{
Bounds VolumeTraits<RegularVolume>::computeBounds(const TransformMatrix &matrix, const RegularVolume &data)
{
    auto size = brayns::Vector3f(data.size) * data.spacing;
    Bounds bounds;
    bounds.expand(matrix.transformPoint(Vector3f(0.f, 0.f, 0.f)));
    bounds.expand(matrix.transformPoint(size));
    return bounds;
}

void VolumeTraits<RegularVolume>::updateData(ospray::cpp::Volume &handle, RegularVolume &data)
{
    const auto cellCentered = !data.perVertexData;
    const auto dataType = static_cast<OSPDataType>(data.dataType);
    const auto &voxels = data.voxels;
    const auto &size = data.size;
    const auto &spacing = data.spacing;

    OSPData sharedData = ospNewSharedData(voxels.data(), dataType, size.x, 0, size.y, 0, size.z);
    handle.setParam(RegularVolumeParameters::data, sharedData);
    ospRelease(sharedData);

    handle.setParam(RegularVolumeParameters::cellCentered, cellCentered);
    handle.setParam(RegularVolumeParameters::gridSpacing, spacing);
}
}
