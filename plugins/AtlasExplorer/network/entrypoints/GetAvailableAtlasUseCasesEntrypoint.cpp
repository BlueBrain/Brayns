/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#include "GetAvailableAtlasUseCasesEntrypoint.h"

#include <brayns/network/common/ExtractModel.h>

#include "common/ExtractAtlas.h"

GetAvailableAtlasUseCasesEntrypoint::GetAvailableAtlasUseCasesEntrypoint(brayns::ModelManager &models)
    : _models(models)
    , _useCases(UseCaseManager::createDefault())
{
}

std::string GetAvailableAtlasUseCasesEntrypoint::getMethod() const
{
    return "get-available-atlas-usecases";
}

std::string GetAvailableAtlasUseCasesEntrypoint::getDescription() const
{
    return "Returns a list of available atlas visualization usecases for the given model";
}

void GetAvailableAtlasUseCasesEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto modelId = params.model_id;
    auto &component = ExtractAtlas::fromId(_models, modelId);
    auto useCases = _useCases.getValidUseCasesForAtlas(*component.atlas);
    request.reply(useCases);
}
