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

#include "VisualizeAtlasUseCaseEntrypoint.h"

#include <brayns/core/engine/components/Transform.h>
#include <brayns/core/network/common/ExtractModel.h>

#include <brayns/atlas/network/entrypoints/common/ExtractAtlas.h>

namespace
{
class AtlasDataCloner
{
public:
    static void clone(const brayns::Model &src, brayns::Model &dst)
    {
        auto &srcComponents = src.getComponents();
        auto &dstComponents = dst.getComponents();

        auto &atlas = srcComponents.get<AtlasData>();
        dstComponents.add<AtlasData>(atlas);

        if (auto transform = srcComponents.find<brayns::Transform>())
        {
            dstComponents.add<brayns::Transform>(*transform);
        }
    }
};
}

VisualizeAtlasUseCaseEntrypoint::VisualizeAtlasUseCaseEntrypoint(brayns::ModelManager &models):
    _models(models),
    _useCases(UseCaseManager::createDefault())
{
}

std::string VisualizeAtlasUseCaseEntrypoint::getMethod() const
{
    return "visualize-atlas-usecase";
}

std::string VisualizeAtlasUseCaseEntrypoint::getDescription() const
{
    return "Visualizes the specified use case based on the atlas data of the given model";
}

void VisualizeAtlasUseCaseEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto modelId = params.model_id;
    auto useCaseName = params.use_case;
    auto useCaseParams = params.params;

    auto &instance = brayns::ExtractModel::fromId(_models, modelId);
    auto &model = instance.getModel();
    auto &component = ExtractAtlas::fromModel(model);
    auto &atlas = *component.atlas;

    auto &useCase = _useCases.getUseCase(useCaseName);
    if (!useCase.isValidAtlas(atlas))
    {
        throw brayns::InvalidParamsException("The use-case is not valid for the given type of atlas");
    }

    auto newModel = useCase.run(atlas, useCaseParams);
    AtlasDataCloner::clone(model, *newModel);
    auto newInstance = _models.add(std::move(newModel));
    request.reply(*newInstance);
}
