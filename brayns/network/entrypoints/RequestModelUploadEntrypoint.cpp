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

#include <brayns/common/Log.h>

#include <brayns/network/common/ErrorHandler.h>
#include <brayns/network/common/ProgressHandler.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace
{
class BinaryParamsValidator
{
public:
    static void validate(const brayns::BinaryParam &params, const brayns::LoaderRegistry &loaders)
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
        if (!loaders.isSupportedType(type))
        {
            throw brayns::InvalidParamsException("Unsupported model type '" + type + "'");
        }
    }
};

class BlobValidator
{
public:
    static void throwIfTooBig(const brayns::BinaryParam &params, std::string_view data)
    {
        auto modelSize = params.size;
        auto frameSize = data.size();
        _throwIfTooBig(modelSize, frameSize);
    }

private:
    static void _throwIfTooBig(size_t modelSize, size_t frameSize)
    {
        if (frameSize != modelSize)
        {
            return;
        }
        std::ostringstream stream;
        stream << "Too many bytes uploaded, model size is " << modelSize << " and frame size is " << frameSize;
        throw brayns::InvalidParamsException(stream.str());
    }
};

using Request = brayns::RequestModelUploadEntrypoint::Request;
using Progress = brayns::ProgressHandler<Request>;

class BlobLoader
{
public:
    static brayns::Blob load(const brayns::BinaryParam &params, std::string_view data, const Progress &progress)
    {
        auto blob = _prepare(params);
        BlobValidator::throwIfTooBig(params, data);
        _load(data, blob);
        progress.notify("Model uploaded", 0.5);
        return blob;
    }

private:
    static brayns::Blob _prepare(const brayns::BinaryParam &params)
    {
        brayns::Blob blob;
        blob.type = params.type;
        blob.name = params.getName();
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
    static const brayns::AbstractLoader &find(const brayns::BinaryParam &params, const brayns::LoaderRegistry &loaders)
    {
        auto &name = params.getLoaderName();
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

class ModelLoader
{
public:
    static std::vector<brayns::ModelDescriptorPtr> loadModels(
        brayns::Scene &scene,
        const brayns::LoaderRegistry &loaders,
        const brayns::BinaryParam &params,
        std::string_view data,
        const Progress &progress)
    {
        auto blob = BlobLoader::load(params, data, progress);
        auto loader = LoaderFinder::find(params, loaders);
        auto parameters = params.getLoadParameters();
        auto callback = [&](auto &operation, auto amount) { progress.notify(operation, 0.5 + amount / 2.0); };
        auto descriptors = loader.loadFromBlob(std::move(blob), {callback}, parameters, scene);
        scene.addModels(descriptors, params);
        return descriptors;
    }
};
} // namespace

namespace brayns
{
RequestModelUploadEntrypoint::RequestModelUploadEntrypoint(
    Scene &scene,
    const LoaderRegistry &loaders,
    CancellationToken token)
    : _scene(scene)
    , _loaders(loaders)
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
    if (_request)
    {
        throw JsonRpcException("Another model upload has already been requested.");
    }
    auto params = request.getParams();
    BinaryParamsValidator::validate(params, _loaders);
    _request = request;
    _params = std::move(params);
}

void RequestModelUploadEntrypoint::onBinary(const ClientRequest &request)
{
    if (!_request)
    {
        Log::warn("Binary frame received while no model uploads requested.");
        return;
    }
    auto data = request.getData();
    try
    {
        auto progress = ProgressHandler(_token, *_request);
        auto descriptors = ModelLoader::loadModels(_scene, _loaders, _params, data, progress);
        _request->reply(descriptors);
    }
    catch (...)
    {
        ErrorHandler::reply(*_request);
    }
}

void RequestModelUploadEntrypoint::onCancel()
{
    _token.cancel();
}

void RequestModelUploadEntrypoint::onDisconnect(const ClientRef &client)
{
    if (_request && client == _request->getClient())
    {
        _token.cancel();
    }
}
} // namespace brayns
