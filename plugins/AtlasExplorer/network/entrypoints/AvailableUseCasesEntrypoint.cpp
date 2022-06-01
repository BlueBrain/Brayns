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

#include "AvailableUseCasesEntrypoint.h"

#include <brayns/network/common/ExtractModel.h>

#include <components/AtlasComponent.h>
#include <network/entrypoints/common/ExtractAtlas.h>

AvailableUseCasesEntrypoint::AvailableUseCasesEntrypoint(brayns::SceneModelManager &modelManager)
    : _modelManager(modelManager)
{
}

std::string AvailableUseCasesEntrypoint::getMethod() const
{
    return "get-available-atlas-usecases";
}

std::string AvailableUseCasesEntrypoint::getDescription() const
{
    return "Returns a list of available atlas visualization usecases for the given model";
}

void AvailableUseCasesEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto modelId = params.model_id;
    const auto &atlas = ExtractAtlas::fromId(_modelManager, modelId);
    auto useCases = _useCases.getValidUseCasesForVolume(atlas);
    request.reply(useCases);
}
