/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "UploadModelEntrypoint.h"

#include <brayns/core/engine/common/AddLoadInfo.h>
#include <brayns/core/engine/common/SimulationScanner.h>

#include <brayns/core/network/common/LoaderHelper.h>
#include <brayns/core/network/common/ProgressHandler.h>

#include <brayns/core/network/jsonrpc/JsonRpcException.h>

namespace
{
class RequestValidator
{
public:
    static void validate(const brayns::UploadModelParams &params, std::string_view data)
    {
        if (params.type.empty())
        {
            throw brayns::InvalidParamsException("No model type");
        }
        if (data.empty())
        {
            throw brayns::InvalidParamsException("Mo model binary data");
        }
    }
};

class LoadInfoFactory
{
public:
    static brayns::LoadInfo create(const brayns::UploadModelParams &parameters)
    {
        auto info = brayns::LoadInfo();
        info.source = brayns::LoadInfo::LoadSource::FromBlob;
        info.loaderName = parameters.loader_name;
        info.loadParameters = parameters.loader_properties;
        return info;
    }
};

class BinaryModelLoader
{
public:
    static void load(
        const brayns::UploadModelEntrypoint::Request &request,
        brayns::ModelManager &manager,
        brayns::LoaderRegistry &loaders,
        brayns::CancellationToken &token)
    {
        auto params = request.getParams();
        auto &data = request.getBinary();

        RequestValidator::validate(params, data);

        auto progress = brayns::ProgressHandler(token, request);

        progress.notify("Model uploaded", 0.5);

        auto &name = params.loader_name;
        auto &properties = params.loader_properties;
        auto &format = params.type;

        auto &loader = brayns::LoaderHelper::findAndValidate(loaders, name, format, properties);

        if (!loader.canLoadBinary())
        {
            throw brayns::InvalidParamsException("Loader '" + loader.getName() + "' cannot load binary");
        }

        auto callback = [&](auto &operation, auto amount) { progress.notify(operation, 0.5 + 0.5 * amount); };
        auto models = loader.loadBinary({params.type, data, callback, properties});

        auto result = manager.add(std::move(models));

        auto loadInfo = LoadInfoFactory::create(params);
        brayns::AddLoadInfo::toInstances(loadInfo, result);

        request.reply(result);
    }
};
} // namespace

namespace brayns
{
UploadModelEntrypoint::UploadModelEntrypoint(
    ModelManager &models,
    LoaderRegistry &loaders,
    SimulationParameters &simulation,
    CancellationToken token):
    _models(models),
    _loaders(loaders),
    _simulation(simulation),
    _token(token)
{
}

std::string UploadModelEntrypoint::getMethod() const
{
    return "upload-model";
}

std::string UploadModelEntrypoint::getDescription() const
{
    return "Upload a model from binary request data and return model descriptors on success";
}

bool UploadModelEntrypoint::isAsync() const
{
    return true;
}

void UploadModelEntrypoint::onRequest(const Request &request)
{
    BinaryModelLoader::load(request, _models, _loaders, _token);
    SimulationScanner::scanAndUpdate(_models, _simulation);
}

void UploadModelEntrypoint::onCancel()
{
    _token.cancel();
}

void UploadModelEntrypoint::onDisconnect()
{
    _token.cancel();
}
} // namespace brayns
