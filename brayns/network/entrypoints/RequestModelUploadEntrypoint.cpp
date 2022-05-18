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

#include "RequestModelUploadEntrypoint.h"

#include <sstream>
#include <thread>

#include <brayns/engine/common/SimulationScanner.h>
#include <brayns/network/common/ProgressHandler.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace
{
using Request = brayns::RequestModelUploadEntrypoint::Request;
using Progress = brayns::ProgressHandler<Request>;

class BinaryParamsValidator
{
public:
    static void validate(const brayns::BinaryLoadParameters &params)
    {
        if (params.size == 0)
        {
            throw brayns::InvalidParamsException("Cannot load an empty model");
        }
        auto &type = params.type;
        if (type.empty())
        {
            throw brayns::InvalidParamsException("Missing model type");
        }
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

class BinaryLock
{
public:
    static brayns::ClientRequest waitForBinary(brayns::BinaryManager &binary, const Progress &progress)
    {
        while (true)
        {
            progress.poll();
            auto request = binary.poll();
            if (request)
            {
                return std::move(*request);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
};

class BlobValidator
{
public:
    static void throwIfTooBig(const brayns::BinaryLoadParameters &params, std::string_view data)
    {
        auto modelSize = params.size;
        auto frameSize = data.size();
        _throwIfTooBig(modelSize, frameSize);
    }

private:
    static void _throwIfTooBig(size_t modelSize, size_t frameSize)
    {
        if (frameSize == modelSize)
        {
            return;
        }
        std::ostringstream stream;
        stream << "Frame size of " << frameSize << " different from model size of " << modelSize;
        throw brayns::InvalidParamsException(stream.str());
    }
};

class BlobLoader
{
public:
    static brayns::Blob
        load(const brayns::BinaryLoadParameters &params, std::string_view data, const Progress &progress)
    {
        auto blob = _prepare(params);
        BlobValidator::throwIfTooBig(params, data);
        _load(data, blob);
        progress.notify("Model uploaded", 0.5);
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

class BinaryModelHandler
{
public:
    BinaryModelHandler(
        brayns::SceneModelManager &modelManager,
        const brayns::LoaderRegistry &loaders,
        brayns::BinaryManager &binary,
        brayns::CancellationToken &token)
        : _modelManager(modelManager)
        , _loaders(loaders)
        , _binary(binary)
        , _token(token)
    {
    }

    void handle(const Request &request)
    {
        auto progress = Progress(_token, request);
        auto params = request.getParams();
        BinaryParamsValidator::validate(params);
        auto &loader = LoaderFinder::find(params, _loaders);
        auto binaryRequest = BinaryLock::waitForBinary(_binary, progress);
        auto data = binaryRequest.getData();
        auto blob = BlobLoader::load(params, data, progress);
        auto parameters = params.loadParameters;
        auto callback = [&](auto &operation, auto amount) { progress.notify(operation, 0.5 + 0.5 * amount); };
        auto models = loader.loadFromBlob(std::move(blob), {callback}, parameters);

        brayns::ModelLoadParameters loadParameters;
        loadParameters.type = brayns::ModelLoadParameters::LoadType::FROM_BLOB;
        loadParameters.loaderName = params.loaderName;
        loadParameters.loadParameters = parameters;

        std::vector<brayns::ModelInstance *> result;
        result.reserve(models.size());
        for (auto &model : models)
        {
            auto &instance = _modelManager.addModel(loadParameters, std::move(model));
            result.push_back(&instance);
        }

        request.reply(result);
    }

private:
    brayns::SceneModelManager &_modelManager;
    const brayns::LoaderRegistry &_loaders;
    brayns::BinaryManager &_binary;
    brayns::CancellationToken &_token;
};
} // namespace

namespace brayns
{
RequestModelUploadEntrypoint::RequestModelUploadEntrypoint(
    Scene &scene,
    const LoaderRegistry &loaders,
    SimulationParameters &simulation,
    BinaryManager &binary,
    CancellationToken token)
    : _scene(scene)
    , _loaders(loaders)
    , _simulation(simulation)
    , _binary(binary)
    , _token(token)
{
}

std::string RequestModelUploadEntrypoint::getMethod() const
{
    return "request-model-upload";
}

std::string RequestModelUploadEntrypoint::getDescription() const
{
    return "Request model upload from next binary frame received and return model descriptors on success";
}

bool RequestModelUploadEntrypoint::isAsync() const
{
    return true;
}

void RequestModelUploadEntrypoint::onRequest(const Request &request)
{
    auto &modelManager = _scene.getModels();
    BinaryModelHandler handler(modelManager, _loaders, _binary, _token);
    SimulationScanner::scanAndUpdate(modelManager, _simulation);
    _scene.computeBounds();
    handler.handle(request);
}

void RequestModelUploadEntrypoint::onPreRender()
{
    _binary.flush();
}

void RequestModelUploadEntrypoint::onCancel()
{
    _token.cancel();
}

void RequestModelUploadEntrypoint::onDisconnect()
{
    _token.cancel();
}
} // namespace brayns
