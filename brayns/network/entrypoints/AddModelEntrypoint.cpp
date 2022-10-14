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

#include <brayns/engine/common/AddLoadInfo.h>
#include <brayns/engine/common/SimulationScanner.h>
#include <brayns/network/common/ProgressHandler.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace
{
class ModelParametersValidator
{
public:
    static void validate(const brayns::AddModelParams &params, const brayns::LoaderRegistry &loaders)
    {
        auto &path = params.path;
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

class LoadInfoFactory
{
public:
    static brayns::LoadInfo create(const brayns::AddModelParams &params)
    {
        auto info = brayns::LoadInfo();
        info.source = brayns::LoadInfo::LoadSource::FromFile;
        info.loaderName = params.loader_name;
        info.loadParameters = params.loader_properties;
        info.path = params.path;
        return info;
    }
};
} // namespace

namespace brayns
{
AddModelEntrypoint::AddModelEntrypoint(
    ModelManager &models,
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
    auto &path = params.path;
    auto &name = params.loader_name;
    auto &loader = _loaders.getSuitableLoader(path, "", name);
    auto &parameters = params.loader_properties;
    auto callback = [&](const auto &operation, auto amount) { progress.notify(operation, amount); };
    auto models = loader.loadFromFile(path, {callback}, parameters);
    auto instances = _models.addModels(std::move(models));

    auto loadInfo = LoadInfoFactory::create(params);
    AddLoadInfo::toInstances(loadInfo, instances);
    SimulationScanner::scanAndUpdate(_models, _simulation);

    request.reply(instances);
}

void AddModelEntrypoint::onCancel()
{
    _token.cancel();
}
} // namespace brayns
