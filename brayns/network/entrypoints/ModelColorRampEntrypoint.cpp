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

#include "ModelColorRampEntrypoint.h"

#include <brayns/network/common/ExtractModel.h>

namespace brayns
{
GetModelTransferFunctionEntrypoint::GetModelTransferFunctionEntrypoint(ModelManager &models)
    : _models(models)
{
}

std::string GetModelTransferFunctionEntrypoint::getMethod() const
{
    return "get-model-transfer-function";
}

std::string GetModelTransferFunctionEntrypoint::getDescription() const
{
    return "Get the transfer function of the given model";
}

void GetModelTransferFunctionEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto modelId = params.id;
    auto &modelInstance = ExtractModel::fromId(_models, modelId);
    auto &model = modelInstance.getModel();
    auto &components = model.getComponents();
    if (auto colorRamp = components.find<ColorRamp>())
    {
        request.reply(*colorRamp);
        return;
    }
    throw JsonRpcException("The requested model does not have a transfer function");
}

SetModelTransferFunctionEntrypoint::SetModelTransferFunctionEntrypoint(ModelManager &models)
    : _models(models)
{
}

std::string SetModelTransferFunctionEntrypoint::getMethod() const
{
    return "set-model-transfer-function";
}

std::string SetModelTransferFunctionEntrypoint::getDescription() const
{
    return "Set the transfer function of the given model";
}

void SetModelTransferFunctionEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto modelId = params.id;
    auto &buffer = params.transfer_function;
    auto &modelInstance = ExtractModel::fromId(_models, modelId);
    auto &model = modelInstance.getModel();
    auto &components = model.getComponents();
    if (auto colorRamp = components.find<ColorRamp>())
    {
        buffer.extract(*colorRamp);
        request.reply(EmptyMessage());
        return;
    }

    throw InvalidRequestException("The requested model does not have a transfer function");
}
} // namespace brayns
