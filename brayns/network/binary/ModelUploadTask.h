/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#pragma once

#include <brayns/engine/Engine.h>
#include <brayns/engine/Scene.h>

#include <brayns/network/adapters/BinaryParamAdapter.h>
#include <brayns/network/adapters/ModelDescriptorAdapter.h>
#include <brayns/network/entrypoint/EntrypointTask.h>
#include <brayns/network/tasks/NetworkTaskMonitor.h>

namespace brayns
{
class ModelUploadTask : public EntrypointTask<BinaryParam, ModelDescriptors>
{
public:
    ModelUploadTask(Engine& engine)
        : _engine(&engine)
    {
    }

    const std::string& getChunksId() const { return _params.chunksID; }

    void addBlob(const std::string& blob)
    {
        _addBlob(blob);
        _checkBlobSize();
        _uploadProgress();
        if (_isReady())
        {
            _monitor.notify();
        }
    }

    virtual void run() override
    {
        _monitor.wait();
        checkCancelled();
        auto& scene = _engine->getScene();
        _descriptors =
            scene.loadModels(std::move(_blob), _params,
                             {[this](const auto& operation, auto amount)
                              { _loadingProgress(operation, amount); }});
    }

    virtual void onStart() override
    {
        _params = getParams();
        _validateParams();
        _blob.type = _params.type;
        _blob.name = _params.getName();
    }

    virtual void onComplete() override
    {
        _engine->triggerRender();
        reply(_descriptors);
    }

    virtual void onDisconnect() override { cancel(); }

    virtual void onCancel() override { _monitor.notify(); }

private:
    void _validateParams()
    {
        if (_params.size == 0)
        {
            throw EntrypointException("Cannot load an empty model");
        }
        auto& type = _params.type;
        if (type.empty())
        {
            throw EntrypointException("Missing model type");
        }
        auto& scene = _engine->getScene();
        auto& registry = scene.getLoaderRegistry();
        if (!registry.isSupportedType(type))
        {
            throw EntrypointException("Unsupported model type '" + type + "'");
        }
    }

    void _addBlob(const std::string& blob)
    {
        auto& data = _blob.data;
        data.insert(data.end(), blob.begin(), blob.end());
    }

    void _checkBlobSize()
    {
        auto modelSize = _params.size;
        auto& data = _blob.data;
        auto blobSize = data.size();
        if (blobSize < modelSize)
        {
            return;
        }
        _tooManyBytesReceived(blobSize, modelSize);
    }

    void _tooManyBytesReceived(size_t blobSize, size_t modelSize)
    {
        cancel();
        std::ostringstream stream;
        stream << "Too many bytes received: ";
        stream << "model size = " << modelSize;
        stream << " total received blob size = " << blobSize;
        throw EntrypointException(stream.str());
    }

    bool _isReady()
    {
        auto modelSize = _params.size;
        auto& data = _blob.data;
        auto blobSize = data.size();
        return blobSize == modelSize;
    }

    void _uploadProgress()
    {
        auto message = "Model upload " + _params.getName() + "...";
        auto modelSize = _params.size;
        auto& data = _blob.data;
        auto blobSize = data.size();
        auto amount = double(blobSize) / double(modelSize);
        progress(message, amount);
    }

    void _loadingProgress(const std::string& operation, double amount)
    {
        progress(operation, 0.5 + 0.5 * amount);
    }

    Engine* _engine;
    BinaryParam _params;
    Blob _blob;
    ModelDescriptors _descriptors;
    NetworkTaskMonitor _monitor;
};

using ModelUploadTaskPtr = std::shared_ptr<ModelUploadTask>;
} // namespace brayns