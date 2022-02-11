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
#include <string_view>
#include <unordered_map>

#include <brayns/network/entrypoint/EntrypointException.h>

#include "ModelUploadTask.h"

namespace brayns
{
/**
 * @brief Request thrown when the chunk has no corresponding model uploads.
 *
 */
class InvalidChunkRequestException : public InvalidRequestException
{
public:
    /**
     * @brief Invalid client with no running tasks.
     *
     * @param client Client ref.
     */
    InvalidChunkRequestException(const ClientRef &client);

    /**
     * @brief Invalid chunk ID with no running tasks.
     *
     * @param client Client ref.
     * @param chunksId Current chunks ID.
     */
    InvalidChunkRequestException(const ClientRef &client, const std::string &chunksId);
};

/**
 * @brief All uploads of one client.
 *
 */
struct ClientUpload
{
    /**
     * @brief Current model upload ID.
     *
     */
    std::string nextChunkId;

    /**
     * @brief Model (chunks) ID -> upload task.
     *
     */
    std::unordered_map<std::string, std::unique_ptr<ModelUploadTask>> tasks;
};

/**
 * @brief Manager for binary model uploads.
 *
 */
class ModelUploadManager
{
public:
    /**
     * @brief Poll upload tasks and remove finished ones.
     *
     */
    void poll();

    /**
     * @brief Register a model upload task.
     *
     * @param task Task to upload model.
     */
    void add(std::unique_ptr<ModelUploadTask> task);

    /**
     * @brief Indicate that the next binary request from the given client will
     * be a chunk of the given model.
     *
     * @param client Client setting next chunk ID.
     * @param id Model chunks ID given at model upload request.
     */
    void setNextChunkId(const ClientRef &client, const std::string &id);

    /**
     * @brief Add the model chunk to the correct model upload task.
     *
     * @param client Client sending binary request.
     * @param data Client binary request data.
     */
    void addChunk(const ClientRef &client, std::string_view data);

private:
    std::unordered_map<ClientRef, ClientUpload> _clientUploads;
};
} // namespace brayns
