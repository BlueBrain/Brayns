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

#include <string_view>
#include <vector>

#include <brayns/common/Blob.h>

#include <brayns/engine/Scene.h>

#include <brayns/io/LoaderRegistry.h>

#include <brayns/network/adapters/BinaryParamAdapter.h>
#include <brayns/network/adapters/ModelDescriptorAdapter.h>
#include <brayns/network/entrypoint/EntrypointTask.h>
#include <brayns/network/tasks/TaskMonitor.h>

namespace brayns
{
/**
 * @brief Data used in the model upload task.
 *
 */
struct ModelUploadContext
{
    Scene *scene = nullptr;
    const LoaderRegistry *loaders = nullptr;
    BinaryParam params;
    bool started = false;
    bool finished = false;
    std::exception_ptr error;
    Blob blob;
    std::vector<ModelDescriptorPtr> descriptors;
    TaskMonitor monitor;
};

/**
 * @brief Task implementation to execute a binary model upload.
 *
 */
class ModelUploadTask : public EntrypointTask<BinaryParam, std::vector<ModelDescriptorPtr>>
{
public:
    /**
     * @brief Construct a task with the model upload request and dependencies.
     *
     * @param request Model upload request
     * @param scene Scene to load model.
     * @param loaders Loaders available to load the model.
     */
    ModelUploadTask(Request request, Scene &scene, const LoaderRegistry &loaders);

    /**
     * @brief Get the ID of the model chunks from the upload request.
     *
     * @return const std::string& Model chunks ID.
     */
    const std::string &getChunksId() const;

    /**
     * @brief Add a new chunk of the model blob data.
     *
     * @param data Chunk data.
     */
    void addChunk(std::string_view data);

protected:
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
    ModelUploadContext _context;
};
} // namespace brayns
