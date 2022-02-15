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

#include <brayns/engine/Engine.h>
#include <brayns/engine/Scene.h>

#include <brayns/io/LoaderRegistry.h>

#include <brayns/network/entrypoint/EntrypointException.h>

namespace brayns
{
ModelUploadTask::ModelUploadTask(Engine &engine, LoaderRegistry &registry)
    : _engine(&engine)
    , _registry(&registry)
{
}

const std::string &ModelUploadTask::getChunksId() const
{
    return _params.chunks_id;
}

void ModelUploadTask::addBlob(const std::string &blob)
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

void ModelUploadTask::run()
{
    _monitor.wait();
    checkCancelled();
    auto &scene = _engine->getScene();

    const auto &blobType = _blob.type;
    const auto &loaderName = _params.loader_name;
    auto &loader = _registry->getSuitableLoader("", blobType, loaderName);

    auto models = loader.loadFromBlob(
        std::move(_blob),
        {[this](const auto &operation, auto amount) { progress(operation, 0.5 + 0.5 * amount); }},
        _params.loader_properties);

    for(auto &model : models)
        scene.addModels(std::move(model.model));
}

void ModelUploadTask::onStart()
{
    _modelUploaded = false;
    _params = getParams();
    _validateParams();
    _blob.type = _params.type;
    _blob.name = _params.name;
    _blob.data.clear();
}

void ModelUploadTask::onComplete()
{
    _engine->triggerRender();
    reply(_descriptors);
}

void ModelUploadTask::onDisconnect()
{
    cancel();
}

void ModelUploadTask::onCancel()
{
    _monitor.notify();
}

size_t ModelUploadTask::_getModelSize() const
{
    return _params.size;
}

size_t ModelUploadTask::_getCurrentSize() const
{
    auto &data = _blob.data;
    return data.size();
}

double ModelUploadTask::_getUploadProgress() const
{
    return double(_getCurrentSize()) / double(_getModelSize());
}

void ModelUploadTask::_validateParams()
{
    if (_params.size == 0)
    {
        throw EntrypointException("Cannot load an empty model");
    }
    auto &type = _params.type;
    if (type.empty())
    {
        throw EntrypointException("Missing model type");
    }
    if (!_registry->isSupportedType(type))
    {
        throw EntrypointException("Unsupported model type '" + type + "'");
    }
}

void ModelUploadTask::_addBlob(const std::string &blob)
{
    _throwIfModelAlreadyUploaded();
    _throwIfBlobIsTooBig(blob);
    _addBlobData(blob);
    _uploadProgress();
    _checkIfUploadIsFinished();
}

void ModelUploadTask::_throwIfModelAlreadyUploaded()
{
    if (_modelUploaded)
    {
        throw EntrypointException("Model already uploaded");
    }
}

void ModelUploadTask::_throwIfBlobIsTooBig(const std::string &blob)
{
    auto modelSize = _getModelSize();
    auto currentSize = _getCurrentSize();
    auto newSize = currentSize + blob.size();
    if (newSize <= modelSize)
    {
        return;
    }
    std::ostringstream stream;
    stream << "Too many bytes uploaded: model size = " << modelSize << " received = " << newSize;
    throw EntrypointException(stream.str());
}

void ModelUploadTask::_addBlobData(const std::string &blob)
{
    auto &data = _blob.data;
    data.insert(data.end(), blob.begin(), blob.end());
}

void ModelUploadTask::_checkIfUploadIsFinished()
{
    if (_getCurrentSize() != _getModelSize())
    {
        return;
    }
    _modelUploaded = true;
    _monitor.notify();
}

void ModelUploadTask::_uploadProgress()
{
    auto message = "Model upload of " + _params.name + "...";
    progress(message, 0.5 * _getUploadProgress());
}

void ModelUploadTask::_loadingProgress(const std::string &operation, double amount)
{
    progress(operation, 0.5 + 0.5 * amount);
}
} // namespace brayns
