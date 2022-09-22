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

#include "UploadModelEntrypoint.h"

#include <brayns/engine/common/SimulationScanner.h>
#include <brayns/network/common/ProgressHandler.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace
{
using Request = brayns::UploadModelEntrypoint::Request;
using Progress = brayns::ProgressHandler<Request>;

class BinaryParamsValidator
{
public:
    static void validate(const brayns::BinaryLoadParameters &params)
    {
        if (params.type.empty())
        {
            throw brayns::InvalidParamsException("Missing model type");
        }
    }
};

class BlobLoader
{
public:
    static brayns::Blob load(const brayns::BinaryLoadParameters &params, std::string_view data)
    {
        auto blob = _prepare(params);
        _load(data, blob);
        return blob;
    }

private:
    static brayns::Blob _prepare(const brayns::BinaryLoadParameters &params)
    {
        brayns::Blob blob;
        blob.type = params.type;
        blob.name = params.loaderName;
        return blob;
    }

    static void _load(std::string_view data, brayns::Blob &blob)
    {
        auto &destination = blob.data;
        auto size = data.size();
        destination.reserve(size);
        destination.insert(destination.end(), data.begin(), data.end());
    }
};

class LoaderFinder
{
public:
    static const brayns::AbstractLoader &find(
        const brayns::BinaryLoadParameters &params,
        const brayns::LoaderRegistry &loaders)
    {
        auto &name = params.loaderName;
        auto &type = params.type;
        try
        {
            return loaders.getSuitableLoader("", type, name);
        }
        catch (const std::runtime_error &e)
        {
            throw brayns::InvalidParamsException(e.what());
        }
    }
};

class BinaryModelHandler
{
public:
    BinaryModelHandler(brayns::Scene &scene, const brayns::LoaderRegistry &loaders, brayns::CancellationToken &token)
        : _scene(scene)
        , _loaders(loaders)
        , _token(token)
    {
    }

    void handle(const Request &request)
    {
        auto params = request.getParams();
        BinaryParamsValidator::validate(params);

        auto progress = Progress(_token, request);
        auto &data = request.getBinary();
        auto blob = BlobLoader::load(params, data);
        progress.notify("Model uploaded", 0.5);

        auto &loader = LoaderFinder::find(params, _loaders);
        auto parameters = params.loadParameters;
        auto callback = [&](auto &operation, auto amount) { progress.notify(operation, 0.5 + 0.5 * amount); };
        auto models = loader.loadFromBlob(blob, {callback}, parameters);

        brayns::ModelLoadParameters loadParameters;
        loadParameters.type = brayns::ModelLoadParameters::LoadType::FromBlob;
        loadParameters.loaderName = params.loaderName;
        loadParameters.loadParameters = parameters;

        auto result = _scene.addModels(std::move(loadParameters), std::move(models));
        request.reply(result);
    }

private:
    brayns::Scene &_scene;
    const brayns::LoaderRegistry &_loaders;
    brayns::CancellationToken &_token;
};
} // namespace

namespace brayns
{
UploadModelEntrypoint::UploadModelEntrypoint(
    Scene &scene,
    const LoaderRegistry &loaders,
    SimulationParameters &simulation,
    CancellationToken token)
    : _scene(scene)
    , _loaders(loaders)
    , _simulation(simulation)
    , _token(token)
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
    SimulationScanner::scanAndUpdate(_scene, _simulation);
    BinaryModelHandler handler(_scene, _loaders, _token);
    handler.handle(request);
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
