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

#include "ModelUploadTask.h"

#include <sstream>

#include <brayns/common/Log.h>

#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace
{
class BinaryParamsValidator
{
public:
    static void validate(const brayns::ModelUploadContext &context)
    {
        auto &params = context.params;
        auto &loaders = *context.loaders;
        return _validate(params, loaders);
    }

private:
    static void _validate(const brayns::BinaryParam &params, const brayns::LoaderRegistry &loaders)
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

class BlobBuilder
{
public:
    static brayns::Blob build(const brayns::ModelUploadContext &context)
    {
        auto &params = context.params;
        return _build(params);
    }

private:
    static brayns::Blob _build(const brayns::BinaryParam &params)
    {
        brayns::Blob blob;
        blob.type = params.type;
        blob.name = params.getName();
        return blob;
    }
};

class UploadValidator
{
public:
    static void throwIfCannotUpload(const brayns::ModelUploadContext &context)
    {
        auto started = context.started;
        auto finished = context.finished;
        _throwIfCannotUpload(started, finished);
    }

private:
    static void _throwIfCannotUpload(bool started, bool finished)
    {
        if (!started)
        {
            throw brayns::InvalidRequestException("Model upload not ready");
        }
        if (finished)
        {
            throw brayns::InvalidRequestException("Model upload already finished");
        }
    }
};

class ChunkSizeValidator
{
public:
    static void throwIfTooBig(std::string_view data, const brayns::ModelUploadContext &context)
    {
        auto &params = context.params;
        auto &blob = context.blob;
        auto modelSize = params.size;
        auto currentSize = blob.data.size();
        auto newSize = currentSize + data.size();
        return _throwIfTooBig(modelSize, newSize);
    }

private:
    static void _throwIfTooBig(size_t modelSize, size_t newSize)
    {
        if (newSize <= modelSize)
        {
            return;
        }
        std::ostringstream stream;
        stream << "Too many bytes uploaded, model size is " << modelSize << " and new size is " << newSize;
        throw brayns::InvalidParamsException(stream.str());
    }
};

class ChunkLoader
{
public:
    static void load(std::string_view data, brayns::ModelUploadContext &context)
    {
        auto &blob = context.blob;
        auto &blobData = blob.data;
        _load(data, blobData);
    }

private:
    static void _load(std::string_view data, std::vector<uint8_t> &blob)
    {
        blob.insert(blob.end(), data.begin(), data.end());
    }
};

using Callback = brayns::LoaderProgress::CallbackFn;

class UploadProgress
{
public:
    static void update(const brayns::ModelUploadContext &context, const Callback &progress)
    {
        auto operation = _getOperation(context);
        auto amount = _getAmount(context);
        progress(operation, amount);
    }

private:
    static std::string _getOperation(const brayns::ModelUploadContext &context)
    {
        auto &params = context.params;
        auto &name = params.getName();
        return "Model upload of " + name + "...";
    }

    static double _getAmount(const brayns::ModelUploadContext &context)
    {
        auto &blob = context.blob;
        auto &params = context.params;
        auto currentSize = blob.data.size();
        auto modelSize = params.size;
        return double(currentSize) / double(modelSize);
    }
};

class UploadStatusUpdater
{
public:
    static void notifyIfFinished(brayns::ModelUploadContext &context)
    {
        _checkIfFinished(context);
        _tryNotify(context);
    }

private:
    static void _checkIfFinished(brayns::ModelUploadContext &context)
    {
        auto &finished = context.finished;
        auto &params = context.params;
        auto &blob = context.blob;
        auto &data = blob.data;
        auto modelSize = params.size;
        auto currentSize = data.size();
        finished = currentSize == modelSize;
    }

    static void _tryNotify(brayns::ModelUploadContext &context)
    {
        auto &finished = context.finished;
        auto &monitor = context.monitor;
        if (finished)
        {
            monitor.notify();
        }
    }
};

class ChunkUploader
{
public:
    static void upload(std::string_view data, brayns::ModelUploadContext &context, const Callback &progress)
    {
        try
        {
            _upload(data, context, progress);
        }
        catch (...)
        {
            _error(context, std::current_exception());
        }
    }

private:
    static void _upload(std::string_view data, brayns::ModelUploadContext &context, const Callback &progress)
    {
        UploadValidator::throwIfCannotUpload(context);
        ChunkSizeValidator::throwIfTooBig(data, context);
        ChunkLoader::load(data, context);
        UploadProgress::update(context, progress);
        UploadStatusUpdater::notifyIfFinished(context);
    }

    static void _error(brayns::ModelUploadContext &context, std::exception_ptr e)
    {
        context.error = std::move(e);
        context.monitor.notify();
    }
};

class ChunkUploadLock
{
public:
    static void waitForAllChunks(brayns::ModelUploadContext &context)
    {
        auto &monitor = context.monitor;
        auto &error = context.error;
        monitor.wait();
        if (error)
        {
            std::rethrow_exception(error);
        }
    }
};

class ModelLoader
{
public:
    static void load(brayns::ModelUploadContext &context, Callback callback)
    {
        auto &descriptors = context.descriptors;
        descriptors = _createModels(context, std::move(callback));
        _addModels(context, descriptors);
    }

private:
    static std::vector<brayns::ModelDescriptorPtr> _createModels(brayns::ModelUploadContext &context, Callback callback)
    {
        auto &scene = *context.scene;
        auto &params = context.params;
        auto &blob = context.blob;
        auto parameters = params.getLoadParameters();
        auto &loader = _getLoader(context);
        return loader.loadFromBlob(std::move(blob), std::move(callback), parameters, scene);
    }

    static const brayns::AbstractLoader &_getLoader(const brayns::ModelUploadContext &context)
    {
        auto &loaders = *context.loaders;
        auto &params = context.params;
        auto &name = params.getLoaderName();
        auto &blob = context.blob;
        auto &type = blob.type;
        return loaders.getSuitableLoader("", type, name);
    }

    static void _addModels(brayns::ModelUploadContext &context, std::vector<brayns::ModelDescriptorPtr> &descriptors)
    {
        auto &scene = *context.scene;
        auto &params = context.params;
        scene.addModels(descriptors, params);
    }
};
} // namespace

namespace brayns
{
ModelUploadTask::ModelUploadTask(Request request, Scene &scene, const LoaderRegistry &loaders)
    : EntrypointTask(std::move(request))
{
    _context.scene = &scene;
    _context.loaders = &loaders;
    _context.params = getParams();
    BinaryParamsValidator::validate(_context);
    _context.blob = BlobBuilder::build(_context);
}

const std::string &ModelUploadTask::getChunksId() const
{
    return _context.params.chunksID;
}

void ModelUploadTask::addChunk(std::string_view data)
{
    auto callback = [this](const auto &operation, auto amount) { progress(operation, 0.5 * amount); };
    ChunkUploader::upload(data, _context, callback);
}

void ModelUploadTask::run()
{
    progress("Waiting for chunks", 0.0);
    ChunkUploadLock::waitForAllChunks(_context);
    progress("All chunks received", 0.5);
    auto callback = [this](const auto &operation, auto amount) { progress(operation, 0.5 + 0.5 * amount); };
    ModelLoader::load(_context, callback);
}

void ModelUploadTask::onStart()
{
    _context.started = true;
    _context.finished = false;
}

void ModelUploadTask::onComplete()
{
    auto &descriptors = _context.descriptors;
    reply(descriptors);
}

void ModelUploadTask::onDisconnect()
{
    cancel();
}

void ModelUploadTask::onCancel()
{
    _context.monitor.notify();
}
} // namespace brayns
