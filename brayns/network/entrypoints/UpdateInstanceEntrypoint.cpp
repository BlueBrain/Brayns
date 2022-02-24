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

#include "UpdateInstanceEntrypoint.h"

#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace brayns
{
UpdateInstanceEntrypoint::UpdateInstanceEntrypoint(Scene &scene)
    : _scene(scene)
{
}

std::string UpdateInstanceEntrypoint::getMethod() const
{
    return "update-instance";
}

std::string UpdateInstanceEntrypoint::getDescription() const
{
    return "Update the model instance with the given values";
}

void UpdateInstanceEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto modelId = params.getModelID();
    auto instanceId = params.getInstanceID();
    auto &model = ExtractModel::fromId(_scene, modelId);
    auto instance = model.getInstance(instanceId);
    if (!instance)
    {
        throw JsonRpcException(
            "Model with ID " + std::to_string(modelId) + " has no instances with ID " + std::to_string(instanceId));
    }
    auto &source = model.getModel();
    source.markInstancesDirty();
    _scene.markModified(false);
    request.getParams(*instance);
    request.reply(EmptyMessage());
}
} // namespace brayns
