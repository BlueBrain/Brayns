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

#pragma once

#include <string>
#include <vector>

#include <brayns/common/Blob.h>

#include <brayns/network/adapters/BinaryParamAdapter.h>
#include <brayns/network/adapters/ModelDescriptorAdapter.h>
#include <brayns/network/entrypoint/EntrypointTask.h>
#include <brayns/network/tasks/NetworkTaskMonitor.h>

namespace brayns
{
class Engine;
class LoaderRegistry;

/**
 * @brief Task implementation to execute a binary model upload.
 *
 */
class ModelUploadTask : public EntrypointTask<BinaryParam, std::vector<ModelDescriptorPtr>>
{
public:
    /**
     * @brief Construct a new task with engine access.
     *
     * @param engine Engine used to create the model.
     */
    ModelUploadTask(Engine &engine, LoaderRegistry &registry);

    /**
     * @brief Get the ID of the model chunks.
     *
     * @return const std::string& Model chunks common ID.
     */
    const std::string &getChunksId() const;

    /**
     * @brief Add a new binary blob to the model source.
     *
     * @param blob Blob binary data.
     */
    void addBlob(const std::string &blob);

    /**
     * @brief Wait for upload finished and perform the model loading.
     *
     */
    virtual void run() override;

    /**
     * @brief Prepare the model upload using request params.
     *
     */
    virtual void onStart() override;

    /**
     * @brief Send reply with created models.
     *
     */
    virtual void onComplete() override;

    /**
     * @brief Cancel the task if the client disconnects.
     *
     */
    virtual void onDisconnect() override;

    /**
     * @brief Stop waiting for upload if cancelled.
     *
     */
    virtual void onCancel() override;

private:
    size_t _getModelSize() const;
    size_t _getCurrentSize() const;
    double _getUploadProgress() const;
    void _validateParams();
    void _addBlob(const std::string &blob);
    void _throwIfModelAlreadyUploaded();
    void _throwIfBlobIsTooBig(const std::string &blob);
    void _addBlobData(const std::string &blob);
    void _checkIfUploadIsFinished();
    void _uploadProgress();
    void _loadingProgress(const std::string &operation, double amount);

    Engine *_engine;
    LoaderRegistry *_registry{nullptr};
    BinaryParam _params;
    Blob _blob;
    std::vector<ModelDescriptorPtr> _descriptors;
    NetworkTaskMonitor _monitor;
    bool _modelUploaded = false;
};

using ModelUploadTaskPtr = std::shared_ptr<ModelUploadTask>;
} // namespace brayns
