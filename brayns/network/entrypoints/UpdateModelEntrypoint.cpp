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

#include "UpdateModelEntrypoint.h"

#include <brayns/network/common/ExtractModel.h>

namespace brayns
{
UpdateModelEntrypoint::UpdateModelEntrypoint(SceneModelManager &modelManager)
    : _modelManager(modelManager)
{
}

std::string UpdateModelEntrypoint::getMethod() const
{
    return "update-model";
}

std::string UpdateModelEntrypoint::getDescription() const
{
    return "Update the model with the given values";
}

void UpdateModelEntrypoint::onRequest(const Request &request)
{
    const auto params = request.getParams();
    auto &model = ExtractModel::fromId(_modelManager, params.model_id);
    ModelInstanceProxy proxy (model);
    params.model.deserialize(proxy);
    request.reply(EmptyMessage());
}
} // namespace brayns
