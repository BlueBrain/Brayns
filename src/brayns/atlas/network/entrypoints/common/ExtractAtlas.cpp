/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: nadir.romanguerrero@epfl.ch
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

#include "ExtractAtlas.h"

#include <brayns/core/network/common/ExtractModel.h>
#include <brayns/core/network/jsonrpc/JsonRpcException.h>

const AtlasData &ExtractAtlas::fromId(brayns::ModelManager &models, uint32_t id)
{
    auto &instance = brayns::ExtractModel::fromId(models, id);
    auto &model = instance.getModel();
    return fromModel(model);
}

const AtlasData &ExtractAtlas::fromModel(brayns::Model &model)
{
    auto component = model.getComponents().find<AtlasData>();
    if (!component)
    {
        throw brayns::InvalidParamsException("The requested model does not have an Atlas component");
    }
    return *component;
}
