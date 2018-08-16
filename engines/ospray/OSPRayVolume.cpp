/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
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

#include "OSPRayVolume.h"

#include <brayns/parameters/VolumeParameters.h>

namespace brayns
{
OSPRayVolume::OSPRayVolume(const Vector3ui& dimensions, const Vector3f& spacing,
                           const DataType type, VolumeParameters& params,
                           OSPTransferFunction transferFunction,
                           const std::string& volumeType)
    : Volume(dimensions, spacing, type)
    , _parameters(params)
    , _volume(ospNewVolume(volumeType.c_str()))
{
    const ospcommon::vec3i ospDim(dimensions.x(), dimensions.y(),
                                  dimensions.z());
    ospSetVec3i(_volume, "dimensions", (osp::vec3i&)ospDim);

    const ospcommon::vec3f ospSpacing(spacing.x(), spacing.y(), spacing.z());
    ospSetVec3f(_volume, "gridSpacing", (osp::vec3f&)ospSpacing);

    switch (type)
    {
    case DataType::FLOAT:
        ospSetString(_volume, "voxelType", "float");
        _ospType = OSP_FLOAT;
        _dataSize = 4;
        break;
    case DataType::DOUBLE:
        ospSetString(_volume, "voxelType", "double");
        _ospType = OSP_DOUBLE;
        _dataSize = 8;
        break;
    case DataType::UINT8:
        ospSetString(_volume, "voxelType", "uchar");
        _ospType = OSP_UINT;
        _dataSize = 1;
        break;
    case DataType::UINT16:
        ospSetString(_volume, "voxelType", "ushort");
        _ospType = OSP_UINT2;
        _dataSize = 2;
        break;
    case DataType::INT16:
        ospSetString(_volume, "voxelType", "short");
        _ospType = OSP_INT2;
        _dataSize = 2;
        break;
    case DataType::UINT32:
    case DataType::INT8:
    case DataType::INT32:
        throw std::runtime_error("Unsupported voxel type " +
                                 std::to_string(int(type)));
    }

    ospSetObject(_volume, "transferFunction", transferFunction);
}

OSPRayVolume::~OSPRayVolume()
{
    ospRelease(_volume);
}

OSPRayBrickedVolume::OSPRayBrickedVolume(const Vector3ui& dimensions,
                                         const Vector3f& spacing,
                                         const DataType type,
                                         VolumeParameters& params,
                                         OSPTransferFunction transferFunction)
    : Volume(dimensions, spacing, type)
    , BrickedVolume(dimensions, spacing, type)
    , OSPRayVolume(dimensions, spacing, type, params, transferFunction,
                   "block_bricked_volume")
{
}

OSPRaySharedDataVolume::OSPRaySharedDataVolume(
    const Vector3ui& dimensions, const Vector3f& spacing, const DataType type,
    VolumeParameters& params, OSPTransferFunction transferFunction)
    : Volume(dimensions, spacing, type)
    , SharedDataVolume(dimensions, spacing, type)
    , OSPRayVolume(dimensions, spacing, type, params, transferFunction,
                   "shared_structured_volume")
{
}

void OSPRayVolume::setDataRange(const Vector2f& range)
{
    ospSet2f(_volume, "voxelRange", range.x(), range.y());
    markModified();
}

void OSPRayBrickedVolume::setBrick(const void* data, const Vector3ui& position,
                                   const Vector3ui& size_)
{
    const ospcommon::vec3i pos{int(position.x()), int(position.y()),
                               int(position.z())};
    const ospcommon::vec3i size{int(size_.x()), int(size_.y()), int(size_.z())};
    ospSetRegion(_volume, const_cast<void*>(data), (osp::vec3i&)pos,
                 (osp::vec3i&)size);
    BrickedVolume::_sizeInBytes += size_.product() * _dataSize;
    markModified();
}

void OSPRaySharedDataVolume::setVoxels(const void* voxels)
{
    OSPData data = ospNewData(SharedDataVolume::_dimensions.product(), _ospType,
                              voxels, OSP_DATA_SHARED_BUFFER);
    SharedDataVolume::_sizeInBytes +=
        SharedDataVolume::_dimensions.product() * _dataSize;
    ospSetData(_volume, "voxelData", data);
    ospRelease(data);
    markModified();
}

void OSPRayVolume::commit()
{
    if (_parameters.isModified())
    {
        ospSet1i(_volume, "gradientShadingEnabled",
                 _parameters.getGradientShading());
        ospSet1f(_volume, "adaptiveMaxSamplingRate",
                 _parameters.getAdaptiveMaxSamplingRate());
        ospSet1i(_volume, "adaptiveSampling",
                 _parameters.getAdaptiveSampling());
        ospSet1i(_volume, "singleShade", _parameters.getSingleShade());
        ospSet1i(_volume, "preIntegration", _parameters.getPreIntegration());
        ospSet1f(_volume, "samplingRate", _parameters.getSamplingRate());
        ospSet3fv(_volume, "specular", &_parameters.getSpecular().x());
        ospSet3fv(_volume, "volumeClippingBoxLower",
                  &_parameters.getClipBox().getMin().x());
        ospSet3fv(_volume, "volumeClippingBoxUpper",
                  &_parameters.getClipBox().getMax().x());
    }
    if (isModified() || _parameters.isModified())
        ospCommit(_volume);
    resetModified();
}
}
