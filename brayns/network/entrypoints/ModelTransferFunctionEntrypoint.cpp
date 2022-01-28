/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include "ModelTransferFunctionEntrypoint.h"

namespace brayns
{
std::string GetModelTransferFunctionEntrypoint::getName() const
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
    auto &engine = getApi().getEngine();
    auto &scene = engine.getScene();
    ExtractModel::fromParams(scene, params);
    auto &transferFunction = scene.getTransferFunction();
    request.reply(transferFunction);
}

std::string SetModelTransferFunctionEntrypoint::getName() const
{
    return "set-model-transfer-function";
}

std::string SetModelTransferFunctionEntrypoint::getDescription() const
{
    return "Set the transfer function of the given model";
}

void SetModelTransferFunctionEntrypoint::onRequest(const Request &request)
{
    auto &engine = getApi().getEngine();
    auto &scene = engine.getScene();
    ModelTransferFunction transferFunction(scene);
    request.getParams(transferFunction);
    request.reply(EmptyMessage());
}
} // namespace brayns
