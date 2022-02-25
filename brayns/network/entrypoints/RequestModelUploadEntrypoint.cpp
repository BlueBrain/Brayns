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

#include <brayns/common/Log.h>

#include <brayns/network/common/ProgressHandler.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace
{
class BinaryParamsValidator
{
public:
    static void validate(const brayns::BinaryParam &params)
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

class BinaryExtractor
{
public:
    static void flush(brayns::BinaryManager &binary)
    {
        auto request = binary.poll();
        if (request)
        {
            _discard(*request);
        }
    }

private:
    static void _discard(const brayns::ClientRequest &request)
    {
        brayns::Log::debug("Binary request {} discarded because no model uploads requested.", request);
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
        stream << "Frame size of " << frameSize << " different from model size of " << modelSize;
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

class BinaryModelLoader
{
public:
    static void load(
        brayns::Scene &scene,
        const brayns::AbstractLoader &loader,
        const Request &uploadRequest,
        const brayns::ClientRequest &binaryRequest,
        const brayns::BinaryParam &params,
        brayns::CancellationToken &token)
    {
        try
        {
            auto descriptors = _load(scene, loader, uploadRequest, binaryRequest, params, token);
            uploadRequest.reply(descriptors);
        }
        catch (const brayns::JsonRpcException &e)
        {
            brayns::Log::info("Upload error: {}.", e);
            uploadRequest.error(e);
        }
        catch (const std::exception &e)
        {
            brayns::Log::error("Unknown upload error: {}.", e.what());
            uploadRequest.error(brayns::InternalErrorException(e.what()));
        }
        catch (...)
        {
            brayns::Log::error("Unknown model upload error.");
            uploadRequest.error(brayns::InternalErrorException("Unknown error"));
        }
    }

private:
    static std::vector<brayns::ModelDescriptorPtr> _load(
        brayns::Scene &scene,
        const brayns::AbstractLoader &loader,
        const Request &uploadRequest,
        const brayns::ClientRequest &binaryRequest,
        const brayns::BinaryParam &params,
        brayns::CancellationToken &token)
    {
        auto progress = Progress(token, uploadRequest);
        auto data = binaryRequest.getData();
        auto blob = BlobLoader::load(params, data, progress);
        auto parameters = params.getLoadParameters();
        auto callback = [&](auto &operation, auto amount) { progress.notify(operation, 0.5 + 0.5 * amount); };
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
    BinaryManager &binary,
    CancellationToken token)
    : _scene(scene)
    , _loaders(loaders)
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
    if (_request)
    {
        throw JsonRpcException("A model upload is already running.");
    }
    auto params = request.getParams();
    BinaryParamsValidator::validate(params);
    _loader = &LoaderFinder::find(params, _loaders);
    _request = request;
    _params = std::move(params);
}

void RequestModelUploadEntrypoint::onPreRender()
{
    if (!_request)
    {
        BinaryExtractor::flush(_binary);
        return;
    }
    auto request = _binary.poll();
    if (!request)
    {
        return;
    }
    Log::info("Upload model from binary request {}.", *request);
    BinaryModelLoader::load(_scene, *_loader, *_request, *request, _params, _token);
    Log::info("Model upload from binary request {} done.", *request);
    _request = std::nullopt;
    _loader = nullptr;
}

void RequestModelUploadEntrypoint::onCancel()
{
    _token.cancel();
    _request = std::nullopt;
    _loader = nullptr;
}

void RequestModelUploadEntrypoint::onDisconnect(const ClientRef &client)
{
    if (_request && client == _request->getClient())
    {
        Log::info("Client {} disconnected, cancel upload.", client);
        onCancel();
    }
}
} // namespace brayns
