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

#include <brayns/engine/models/VolumetricModel.h>

namespace
{
struct TransferFunctionHandler
{
    static void commit(OSPTransferFunction &handle, const brayns::TransferFunction &tranferFunction)
    {
        if(handle)
        {
            ospRelease(handle);
        }

        handle = ospNewTransferFunction("piecewiseLinear");

        const auto &colors = tranferFunction.getColors();
        const auto &opacities = tranferFunction.getOpacities();
        const auto &range = tranferFunction.getValuesRange();

        OSPData sharedColors = ospNewSharedData(colors.data(), OSPDataType::OSP_VEC3F, colors.size());
        OSPData sharedOpacities = ospNewSharedData(opacities.data(), OSPDataType::OSP_FLOAT, opacities.size());

        ospSetParam(handle, "color", OSPDataType::OSP_DATA, &sharedColors);
        ospSetParam(handle, "opacity", OSPDataType::OSP_DATA, &sharedOpacities);
        ospSetParam(handle, "valueRange", OSPDataType::OSP_VEC2F, &range);

        ospRelease(sharedColors);
        ospRelease(sharedOpacities);

        ospCommit(handle);
    }
};
}

namespace brayns
{

VolumetricModel::VolumetricModel()
{
    _handle = ospNewVolumetricModel();
}

VolumetricModel::~VolumetricModel()
{
    if(_handle)
        ospRelease(_handle);
}

float VolumetricModel::getDensityScale() const noexcept
{
    return _densityScale;
}

void VolumetricModel::setDensityScale(const float scale) noexcept
{
    _updateValue(_densityScale, scale);
}


void VolumetricModel::setTransferFunction(TransferFunction tf)
{
    _transferFunction = std::move(tf);
    markModified(false);
}

const TransferFunction& VolumetricModel::getTransferFunction() const noexcept
{
    return _transferFunction;
}

void VolumetricModel::commit()
{
    if(_transferFunction.isModified())
    {
        TransferFunctionHandler::commit(_tfHandle, _transferFunction);
        ospSetParam(_handle, "transferFunction", OSPDataType::OSP_TRANSFER_FUNCTION, &_tfHandle);
    }

    ospSetParam(_handle, "densityScale", OSPDataType::OSP_FLOAT, &_densityScale);

    commitVolumeModel();

    ospCommit(_handle);

    auto ospGroup = groupHandle();

    std::vector<OSPVolumetricModel> modelList = {_handle};

    auto sharedModelList = ospNewSharedData(modelList.data(), OSPDataType::OSP_VOLUMETRIC_MODEL, 1);
    auto copyModelList = ospNewData(OSPDataType::OSP_VOLUMETRIC_MODEL, 1);
    ospCopyData(sharedModelList, copyModelList);

    ospSetParam(ospGroup, "volume", OSPDataType::OSP_DATA, &copyModelList);

    ospRelease(sharedModelList);
    ospRelease(copyModelList);

    ospCommit(ospGroup);
}

OSPVolumetricModel VolumetricModel::handle() const noexcept
{
    return _handle;
}

}
