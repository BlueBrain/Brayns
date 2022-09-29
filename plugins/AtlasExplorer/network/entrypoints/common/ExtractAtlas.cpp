/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>

const AtlasVolume &ExtractAtlas::atlasFromId(brayns::ModelManager &models, uint32_t id)
{
    return *componentFromId(models, id).volume;
}

const AtlasVolume &ExtractAtlas::atlasFromModel(brayns::Model &model)
{
    return *componentFromModel(model).volume;
}

const AtlasData &ExtractAtlas::componentFromId(brayns::ModelManager &models, uint32_t id)
{
    auto &instance = brayns::ExtractModel::fromId(models, id);
    auto &model = instance.getModel();
    return componentFromModel(model);
}

const AtlasData &ExtractAtlas::componentFromModel(brayns::Model &model)
{
    auto component = model.getComponents().find<AtlasData>();
    if (!component)
    {
        throw brayns::InvalidParamsException("The requested model does not have an Atlas component");
    }
    return *component;
}
