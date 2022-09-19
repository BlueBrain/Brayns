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
class RequestValidator
{
public:
    static void validate(const brayns::BinaryLoadParameters &params, std::string_view data)
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
<<<<<<< HEAD:brayns/network/entrypoints/UploadModelEntrypoint.cpp
    BinaryModelHandler(brayns::Scene &scene, const brayns::LoaderRegistry &loaders, brayns::CancellationToken &token)
        : _scene(scene)
=======
    BinaryModelHandler(
        brayns::ModelManager &models,
        const brayns::LoaderRegistry &loaders,
        brayns::BinaryManager &binary,
        brayns::CancellationToken &token)
        : _models(models)
>>>>>>> Checkpoint:brayns/network/entrypoints/RequestModelUploadEntrypoint.cpp
        , _loaders(loaders)
        , _token(token)
    {
    }

    void handle(const brayns::UploadModelEntrypoint::Request &request)
    {
        auto params = request.getParams();
        auto &data = request.getBinary();

        RequestValidator::validate(params, data);

        auto progress = brayns::ProgressHandler(_token, request);
        auto blob = BlobLoader::load(params, data);
        progress.notify("Model uploaded", 0.5);

        auto &loader = LoaderFinder::find(params, _loaders);
        auto parameters = params.loadParameters;
        auto callback = [&](auto &operation, auto amount) { progress.notify(operation, 0.5 + 0.5 * amount); };
        auto models = loader.loadFromBlob(blob, {callback}, parameters);
        auto result = _models.addModels(std::move(models));
        request.reply(result);
    }

private:
    brayns::ModelManager &_models;
    const brayns::LoaderRegistry &_loaders;
    brayns::CancellationToken &_token;
};
} // namespace

namespace brayns
{
<<<<<<< HEAD:brayns/network/entrypoints/UploadModelEntrypoint.cpp
UploadModelEntrypoint::UploadModelEntrypoint(
    Scene &scene,
=======
RequestModelUploadEntrypoint::RequestModelUploadEntrypoint(
    ModelManager &models,
>>>>>>> Checkpoint:brayns/network/entrypoints/RequestModelUploadEntrypoint.cpp
    const LoaderRegistry &loaders,
    SimulationParameters &simulation,
    CancellationToken token)
    : _models(models)
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
<<<<<<< HEAD:brayns/network/entrypoints/UploadModelEntrypoint.cpp
    SimulationScanner::scanAndUpdate(_scene, _simulation);
    BinaryModelHandler handler(_scene, _loaders, _token);
=======
    BinaryModelHandler handler(_models, _loaders, _binary, _token);
    SimulationScanner::scanAndUpdate(_models, _simulation);
>>>>>>> Checkpoint:brayns/network/entrypoints/RequestModelUploadEntrypoint.cpp
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
