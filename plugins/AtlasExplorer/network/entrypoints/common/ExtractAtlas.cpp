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

#include <components/AtlasComponent.h>
#include <components/AtlasReferenceComponent.h>

const AtlasVolume &ExtractAtlas::fromId(brayns::SceneModelManager &modelManager, uint32_t id)
{
    auto &instance = brayns::ExtractModel::fromId(modelManager, id);
    auto &model = instance.getModel();
    try
    {
        return fromModel(model);
    }
    catch (...)
    {
    }

    try
    {
        auto &component = model.getComponent<AtlasReferenceComponent>();
        return fromId(modelManager, component.getSourceModelId());
    }
    catch (...)
    {
        throw brayns::InvalidParamsException("The requested model does not have an Atlas component");
    }
}

const AtlasVolume &ExtractAtlas::fromModel(brayns::Model &model)
{
    try
    {
        auto &component = model.getComponent<AtlasComponent>();
        return component.getVolume();
    }
    catch (...)
    {
        throw brayns::InvalidParamsException("The requested model does not have an Atlas component");
    }
}
