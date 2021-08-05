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

#include <sstream>

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

    size_t getModelSize() const { return _params.size; }

    size_t getCurrentSize() const
    {
        auto& data = _blob.data;
        return data.size();
    }

    double getUploadProgress() const
    {
        return double(getCurrentSize()) / double(getModelSize());
    }

    void addBlob(const std::string& blob)
    {
        try
        {
            _addBlob(blob);
        }
        catch (...)
        {
            cancelWith(std::current_exception());
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
        _modelUploaded = false;
        _params = getParams();
        _validateParams();
        _blob.type = _params.type;
        _blob.name = _params.getName();
        _blob.data.clear();
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
        _throwIfModelAlreadyUploaded();
        _throwIfBlobIsTooBig(blob);
        _addBlobData(blob);
        _uploadProgress();
        _checkIfUploadIsFinished();
    }

    void _throwIfModelAlreadyUploaded()
    {
        if (_modelUploaded)
        {
            throw EntrypointException("Model already uploaded");
        }
    }

    void _throwIfBlobIsTooBig(const std::string& blob)
    {
        auto modelSize = getModelSize();
        auto currentSize = getCurrentSize();
        auto newSize = currentSize + blob.size();
        if (newSize <= modelSize)
        {
            return;
        }
        std::ostringstream stream;
        stream << "Too many bytes uploaded: model size = " << modelSize
               << " received = " << newSize;
        throw EntrypointException(stream.str());
    }

    void _addBlobData(const std::string& blob)
    {
        auto& data = _blob.data;
        data.insert(data.end(), blob.begin(), blob.end());
    }

    void _checkIfUploadIsFinished()
    {
        if (getCurrentSize() != getModelSize())
        {
            return;
        }
        _modelUploaded = true;
        _monitor.notify();
    }

    void _uploadProgress()
    {
        auto message = "Model upload of " + _params.getName() + "...";
        progress(message, 0.5 * getUploadProgress());
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
    bool _modelUploaded = false;
};

using ModelUploadTaskPtr = std::shared_ptr<ModelUploadTask>;
} // namespace brayns