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

#include "VisualizeUseCaseEntrypoint.h"

#include <network/entrypoints/common/ExtractAtlas.h>
#include <network/entrypoints/common/GenerateAtlasReference.h>

VisualizeUseCaseEntrypoint::VisualizeUseCaseEntrypoint(brayns::SceneModelManager &models)
    : _modelManager(models)
{
}

std::string VisualizeUseCaseEntrypoint::getMethod() const
{
    return "visualize-atlas-usecase";
}

std::string VisualizeUseCaseEntrypoint::getDescription() const
{
    return "Visualizes the specified use case based on the atlas data of the given model";
}

void VisualizeUseCaseEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();

    auto modelId = params.model_id;
    auto useCase = params.use_case;
    auto useCaseParams = params.params;

    auto &atlas = ExtractAtlas::fromId(_modelManager, modelId);
    auto newModel = _useCases.executeUseCase(useCase, atlas, useCaseParams);
    GenerateAtlasReference::generate(_modelManager, modelId, *newModel);
    auto &instance = _modelManager.addModel({}, std::move(newModel));
    request.reply(instance);
}
