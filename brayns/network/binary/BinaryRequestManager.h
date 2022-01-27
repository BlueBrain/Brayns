/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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
#include <unordered_map>

#include <brayns/network/socket/ConnectionHandle.h>

#include "ModelUploadTask.h"

namespace brayns
{
/**
 * @brief Model upload manager for one client.
 *
 */
class ModelUploader
{
public:
    /**
     * @brief Check if the client has model uploads running.
     *
     * @return true At least one model upload task is running.
     * @return false No model is uploaded.
     */
    bool hasTasks() const;

    /**
     * @brief Add a new model upload task.
     *
     * Task model chunks ID must not be in use by another running model upload.
     *
     * @param task Running model upload task.
     */
    void addTask(ModelUploadTaskPtr task);

    /**
     * @brief Associate the next binary packet with a model.
     *
     * @param id Chunks ID of the model.
     */
    void setNextChunkId(const std::string &id);

    /**
     * @brief Add a new binary packet to the current model.
     *
     * Current model is set using its chunks ID with setNextChunkId.
     *
     * @param blob Model binary data chunk.
     */
    void addBlob(const std::string &blob);

    /**
     * @brief Remove all finished tasks (cancelled or complete).
     *
     */
    void removeFinishedTasks();

private:
    std::string _nextChunkId;
    std::unordered_map<std::string, ModelUploadTaskPtr> _tasks;
};

/**
 * @brief Binary requests manager to handle model upload from client.
 *
 */
class BinaryRequestManager
{
public:
    /**
     * @brief Called on each update to remove finished tasks.
     *
     */
    void pollTasks();

    /**
     * @brief Register a new running model upload task.
     *
     * @param handle Client handle.
     * @param task Task to manage.
     */
    void addTask(const ConnectionHandle &handle, ModelUploadTaskPtr task);

    /**
     * @brief Indicate that the next binary request from the given client will
     * be a chunk of the given model.
     *
     * @param handle Client handle.
     * @param id Model chunks ID given at model upload request.
     */
    void setNextChunkId(const ConnectionHandle &handle, const std::string &id);

    /**
     * @brief Add the binary blob to the correct running mode upload task.
     *
     * @param handle Client handle.
     * @param packet Client binary message.
     */
    void processBinaryRequest(const ConnectionHandle &handle, const InputPacket &packet);

private:
    std::unordered_map<ConnectionHandle, ModelUploader> _uploaders;
};
} // namespace brayns