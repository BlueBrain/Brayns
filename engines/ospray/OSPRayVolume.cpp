/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <engines/ospray/utils.h>

namespace brayns
{
OSPRayVolume::OSPRayVolume(const Vector3ui& dimensions, const Vector3f& spacing,
                           const VolumeDataType type,
                           const VolumeParameters& params,
                           OSPTransferFunction transferFunction,
                           const std::string& volumeType)
    : Volume(dimensions, spacing, type)
    , _parameters(params)
    , _volume(ospNewVolume(volumeType.c_str()))
{
    osphelper::set(_volume, "dimensions", Vector3i(dimensions));
    osphelper::set(_volume, "gridSpacing", Vector3f(spacing));

    switch (type)
    {
    case VolumeDataType::FLOAT:
        osphelper::set(_volume, "voxelType", "float");
        _ospType = OSP_FLOAT;
        _dataSize = 4;
        break;
    case VolumeDataType::DOUBLE:
        osphelper::set(_volume, "voxelType", "double");
        _ospType = OSP_DOUBLE;
        _dataSize = 8;
        break;
    case VolumeDataType::UINT8:
        osphelper::set(_volume, "voxelType", "uchar");
        _ospType = OSP_UINT;
        _dataSize = 1;
        break;
    case VolumeDataType::UINT16:
        osphelper::set(_volume, "voxelType", "ushort");
        _ospType = OSP_UINT2;
        _dataSize = 2;
        break;
    case VolumeDataType::INT16:
        osphelper::set(_volume, "voxelType", "short");
        _ospType = OSP_INT2;
        _dataSize = 2;
        break;
    case VolumeDataType::UINT32:
    case VolumeDataType::INT8:
    case VolumeDataType::INT32:
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
                                         const VolumeDataType type,
                                         const VolumeParameters& params,
                                         OSPTransferFunction transferFunction)
    : Volume(dimensions, spacing, type)
    , BrickedVolume(dimensions, spacing, type)
    , OSPRayVolume(dimensions, spacing, type, params, transferFunction,
                   "block_bricked_volume")
{
}

OSPRaySharedDataVolume::OSPRaySharedDataVolume(
    const Vector3ui& dimensions, const Vector3f& spacing,
    const VolumeDataType type, const VolumeParameters& params,
    OSPTransferFunction transferFunction)
    : Volume(dimensions, spacing, type)
    , SharedDataVolume(dimensions, spacing, type)
    , OSPRayVolume(dimensions, spacing, type, params, transferFunction,
                   "shared_structured_volume")
{
}

void OSPRayVolume::setDataRange(const Vector2f& range)
{
    osphelper::set(_volume, "voxelRange", range);
    markModified();
}

void OSPRayBrickedVolume::setBrick(const void* data, const Vector3ui& position,
                                   const Vector3ui& size_)
{
    const ospcommon::vec3i pos{int(position.x), int(position.y),
                               int(position.z)};
    const ospcommon::vec3i size{int(size_.x), int(size_.y), int(size_.z)};
    ospSetRegion(_volume, const_cast<void*>(data), (osp::vec3i&)pos,
                 (osp::vec3i&)size);
    BrickedVolume::_sizeInBytes += glm::compMul(size_) * _dataSize;
    markModified();
}

void OSPRaySharedDataVolume::setVoxels(const void* voxels)
{
    OSPData data = ospNewData(glm::compMul(SharedDataVolume::_dimensions),
                              _ospType, voxels, OSP_DATA_SHARED_BUFFER);
    SharedDataVolume::_sizeInBytes +=
        glm::compMul(SharedDataVolume::_dimensions) * _dataSize;
    ospSetData(_volume, "voxelData", data);
    ospRelease(data);
    markModified();
}

void OSPRayVolume::commit()
{
    if (_parameters.isModified())
    {
        osphelper::set(_volume, "gradientShadingEnabled",
                       _parameters.getGradientShading());
        osphelper::set(_volume, "adaptiveMaxSamplingRate",
                       static_cast<float>(
                           _parameters.getAdaptiveMaxSamplingRate()));
        osphelper::set(_volume, "adaptiveSampling",
                       _parameters.getAdaptiveSampling());
        osphelper::set(_volume, "singleShade", _parameters.getSingleShade());
        osphelper::set(_volume, "preIntegration",
                       _parameters.getPreIntegration());
        osphelper::set(_volume, "samplingRate",
                       static_cast<float>(_parameters.getSamplingRate()));
        osphelper::set(_volume, "specular",
                       Vector3f(_parameters.getSpecular()));
        osphelper::set(_volume, "volumeClippingBoxLower",
                       Vector3f(_parameters.getClipBox().getMin()));
        osphelper::set(_volume, "volumeClippingBoxUpper",
                       Vector3f(_parameters.getClipBox().getMax()));
    }
    if (isModified() || _parameters.isModified())
        ospCommit(_volume);
    resetModified();
}
} // namespace brayns
