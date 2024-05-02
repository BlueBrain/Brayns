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

#include "GetModelEntrypoint.h"

#include <brayns/core/network/common/ExtractModel.h>

namespace brayns
{
GetModelEntrypoint::GetModelEntrypoint(ModelManager &models):
    _models(models)
{
}

std::string GetModelEntrypoint::getMethod() const
{
    return "get-model";
}

std::string GetModelEntrypoint::getDescription() const
{
    return "Get all the information of the given model";
}

void GetModelEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto modelId = params.id;
    auto &instance = ExtractModel::fromId(_models, modelId);
    request.reply(instance);
}
} // namespace brayns