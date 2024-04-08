/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "ColorRampEntrypoint.h"

#include <brayns/core/network/common/ExtractModel.h>

namespace brayns
{
GetColorRampEntrypoint::GetColorRampEntrypoint(ModelManager &models):
    _models(models)
{
}

std::string GetColorRampEntrypoint::getMethod() const
{
    return "get-color-ramp";
}

std::string GetColorRampEntrypoint::getDescription() const
{
    return "Get the color ramp of the given model";
}

void GetColorRampEntrypoint::onRequest(const Request &request)
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
    throw JsonRpcException("The requested model does not have a color ramp");
}

SetColorRampEntrypoint::SetColorRampEntrypoint(ModelManager &models):
    _models(models)
{
}

std::string SetColorRampEntrypoint::getMethod() const
{
    return "set-color-ramp";
}

std::string SetColorRampEntrypoint::getDescription() const
{
    return "Set the color ramp of the given model";
}

void SetColorRampEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto modelId = params.id;
    auto &buffer = params.color_ramp;
    auto &modelInstance = ExtractModel::fromId(_models, modelId);
    auto &model = modelInstance.getModel();
    auto &components = model.getComponents();
    if (auto colorRamp = components.find<ColorRamp>())
    {
        buffer.extract(*colorRamp);
        request.reply(EmptyJson());
        return;
    }

    throw InvalidRequestException("The requested model does not have a color ramp");
}
} // namespace brayns
