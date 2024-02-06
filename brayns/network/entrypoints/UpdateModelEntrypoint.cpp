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

#include "UpdateModelEntrypoint.h"

#include <brayns/network/common/ExtractModel.h>

namespace brayns
{
UpdateModelEntrypoint::UpdateModelEntrypoint(ModelManager &models):
    _models(models)
{
}

std::string UpdateModelEntrypoint::getMethod() const
{
    return "update-model";
}

std::string UpdateModelEntrypoint::getDescription() const
{
    return "Update the model with the given values and return its new state";
}

void UpdateModelEntrypoint::onRequest(const Request &request)
{
    const auto params = request.getParams();
    auto modelId = params.model_id;
    auto &buffer = params.model;
    auto &model = ExtractModel::fromId(_models, modelId);
    buffer.extract(model);
    request.reply(model);
}
} // namespace brayns
