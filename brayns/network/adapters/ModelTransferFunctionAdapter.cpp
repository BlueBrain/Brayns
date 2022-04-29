/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "ModelTransferFunctionAdapter.h"

#include <brayns/engine/components/TransferFunctionComponent.h>
#include <brayns/network/common/ExtractModel.h>

namespace brayns
{
ModelTransferFunction::ModelTransferFunction(SceneModelManager &smm)
    : _smm(&smm)
{
}

void ModelTransferFunction::setId(const uint32_t id)
{
    _modelId = id;
}

void ModelTransferFunction::setTransferFunction(const JsonBuffer<TransferFunction> &buffer)
{
    auto &modelInstance = _smm->getModelInstance(_modelId);
    auto &model = modelInstance.getModel();

    try
    {
        auto &tfComponent = model.getComponent<TransferFunctionComponent>();
        auto &transferFunction = tfComponent.getTransferFunction();
        buffer.deserialize(transferFunction);
    }
    catch (...)
    {
        throw std::runtime_error("The requested model does not have a transfer function");
    }
}
} // namespace brayns
