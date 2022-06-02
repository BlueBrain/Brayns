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

#include "GenerateAtlasReference.h"

#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>

#include <components/AtlasComponent.h>
#include <components/AtlasReferenceComponent.h>

namespace
{
class AtlasSourceModelIdFinder
{
public:
    static uint32_t find(brayns::SceneModelManager &manager, uint32_t sourceModelId)
    {
        auto &instance = brayns::ExtractModel::fromId(manager, sourceModelId);
        auto &model = instance.getModel();
        try
        {
            model.getComponent<AtlasComponent>();
            return sourceModelId;
        }
        catch (...)
        {
        }

        try
        {
            auto &component = model.getComponent<AtlasReferenceComponent>();
            return component.getSourceModelId();
        }
        catch (...)
        {
        }

        throw brayns::InvalidParamsException("The requested model does not have an Atlas component");
    }
};
}

void GenerateAtlasReference::generate(
    brayns::SceneModelManager &modelManager,
    uint32_t sourceModelId,
    brayns::Model &target)
{
    auto modelId = AtlasSourceModelIdFinder::find(modelManager, sourceModelId);
    target.addComponent<AtlasReferenceComponent>(modelId);
}
