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

#include "TransferFunctionRendererComponent.h"

#include <brayns/engine/Model.h>
#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/components/TransferFunctionComponent.h>

namespace brayns
{
TransferFunctionRendererComponent::TransferFunctionRendererComponent()
{
    _handle = ospNewTransferFunction("piecewiseLinear");
}

void TransferFunctionRendererComponent::onDestroyed()
{
    if (_handle)
    {
        ospRelease(_handle);
        _handle = nullptr;
    }
}

bool TransferFunctionRendererComponent::manualCommit()
{
    Model &model = getModel();
    auto &tf = ExtractModelObject::extractTransferFunction(model);
    if (!tf.isModified())
    {
        return false;
    }

    auto &color = tf.getColors();
    auto &opacity = tf.getOpacities();
    auto &range = tf.getValuesRange();

    auto colorBuffer = DataHandler::shareBuffer(color, OSPDataType::OSP_VEC3F);
    auto opacityBuffer = DataHandler::shareBuffer(opacity, OSPDataType::OSP_FLOAT);

    ospSetParam(_handle, "color", OSPDataType::OSP_DATA, &colorBuffer.handle);
    ospSetParam(_handle, "opacity", OSPDataType::OSP_DATA, &opacityBuffer.handle);
    ospSetParam(_handle, "valueRange", OSPDataType::OSP_VEC2F, &range);

    ospCommit(_handle);

    tf.resetModified();

    return true;
}

OSPTransferFunction TransferFunctionRendererComponent::handle() const noexcept
{
    return _handle;
}
}
