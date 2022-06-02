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

#include "AddModelEntrypoint.h"

#include <brayns/engine/common/SimulationScanner.h>
#include <brayns/network/common/ProgressHandler.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace
{
class ModelParametersValidator
{
public:
    static void validate(const brayns::FileLoadParameters &params, const brayns::LoaderRegistry &loaders)
    {
        auto &path = params.filePath;
        if (path.empty())
        {
            throw brayns::InvalidParamsException("Missing model path");
        }
        if (!loaders.isSupportedFile(path))
        {
            throw brayns::InvalidParamsException("Unsupported file type: '" + path + "'");
        }
    }
};
} // namespace

namespace brayns
{
AddModelEntrypoint::AddModelEntrypoint(
    SceneModelManager &models,
    LoaderRegistry &loaders,
    SimulationParameters &simulation,
    CancellationToken token)
    : _models(models)
    , _loaders(loaders)
    , _simulation(simulation)
    , _token(token)
{
}

std::string AddModelEntrypoint::getMethod() const
{
    return "add-model";
}

std::string AddModelEntrypoint::getDescription() const
{
    return "Add model from path and return model descriptor on success";
}

bool AddModelEntrypoint::isAsync() const
{
    return true;
}

void AddModelEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    ModelParametersValidator::validate(params, _loaders);
    auto progress = brayns::ProgressHandler(_token, request);
    auto &path = params.filePath;
    auto &name = params.loaderName;
    auto &loader = _loaders.getSuitableLoader(path, "", name);
    auto &parameters = params.loadParameters;
    auto callback = [&](const auto &operation, auto amount) { progress.notify(operation, amount); };
    auto models = loader.loadFromFile(path, {callback}, parameters);

    ModelLoadParameters loadParameters;
    loadParameters.type = ModelLoadParameters::LoadType::FROM_FILE;
    loadParameters.path = path;
    loadParameters.loaderName = name;
    loadParameters.loadParameters = parameters;

    auto result = _models.addModels(std::move(loadParameters), std::move(models));

    SimulationScanner::scanAndUpdate(_models, _simulation);

    request.reply(result);
}

void AddModelEntrypoint::onCancel()
{
    _token.cancel();
}
} // namespace brayns
