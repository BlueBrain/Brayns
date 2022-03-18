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

#include <brayns/engine/Model.h>
#include <brayns/engine/defaultcomponents/TransferFunctionComponent.h>
#include <brayns/engine/defaultcomponents/TransferFunctionRendererComponent.h>

namespace brayns
{
TransferFunctionRendererComponent::TransferFunctionRendererComponent()
{
    _handle = ospNewTransferFunction("piecewiseLinear");
}

void TransferFunctionRendererComponent::onStart()
{
    // Check if there is already a transfer function component. Otherwise, add it
    Model& model = getModel();
    try
    {
        model.getComponent<TransferFunctionComponent>();
    }
    catch(...)
    {
        model.addComponent<TransferFunctionComponent>();
    }
}

void TransferFunctionRendererComponent::onDestroyed()
{
    if(_handle)
    {
        ospRelease(_handle);
        _handle = nullptr;
    }
}

bool TransferFunctionRendererComponent::manualCommit()
{
    Model& model = getModel();
    auto& tfComponent = model.getComponent<TransferFunctionComponent>();
    auto& tf = tfComponent.getTransferFunction();

    if(!tf.isModified())
    {
        return false;
    }

    auto& color = tf.getColors();
    OSPData colorData = ospNewSharedData(color.data(), OSPDataType::OSP_VEC3F, color.size());

    auto& opacity = tf.getOpacities();
    OSPData opacityData = ospNewSharedData(opacity.data(), OSPDataType::OSP_FLOAT, opacity.size());

    auto& range = tf.getValuesRange();

    ospSetParam(_handle, "color", OSPDataType::OSP_DATA, &colorData);
    ospSetParam(_handle, "opacity", OSPDataType::OSP_DATA, &opacityData);
    ospSetParam(_handle, "valueRange", OSPDataType::OSP_VEC2F, &range);

    ospRelease(colorData);
    ospRelease(opacityData);

    ospCommit(_handle);

    tf.resetModified();

    return true;
}

OSPTransferFunction TransferFunctionRendererComponent::handle() const noexcept
{
    return _handle;
}
}
