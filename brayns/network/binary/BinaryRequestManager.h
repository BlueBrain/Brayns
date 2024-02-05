/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/common/Log.h>

#include <brayns/network/entrypoint/EntrypointException.h>
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
    bool hasTasks() const { return !_tasks.empty(); }

    /**
     * @brief Add a new model upload task.
     *
     * Task model chunks ID must not be in use by another running model upload.
     *
     * @param task Running model upload task.
     */
    void addTask(ModelUploadTaskPtr task)
    {
        auto& chunksId = task->getChunksId();
        auto& currentTask = _tasks[chunksId];
        if (currentTask)
        {
            throw EntrypointException("A model upload with chunks ID '" +
                                      chunksId + "' is already running");
        }
        _nextChunkId = chunksId;
        currentTask = std::move(task);
    }

    /**
     * @brief Associate the next binary packet with a model.
     *
     * @param id Chunks ID of the model.
     */
    void setNextChunkId(const std::string& id) { _nextChunkId = id; }

    /**
     * @brief Add a new binary packet to the current model.
     *
     * Current model is set using its chunks ID with setNextChunkId.
     *
     * @param blob Model binary data chunk.
     */
    void addBlob(const std::string& blob)
    {
        auto i = _tasks.find(_nextChunkId);
        if (i == _tasks.end())
        {
            Log::error("No model upload with chunks ID '{}'.", _nextChunkId);
            return;
        }
        auto& task = *i->second;
        task.addBlob(blob);
    }

    /**
     * @brief Remove all finished tasks (cancelled or complete).
     *
     */
    void removeFinishedTasks()
    {
        for (auto i = _tasks.begin(); i != _tasks.end();)
        {
            auto& task = *i->second;
            if (task.isRunning())
            {
                ++i;
                continue;
            }
            i = _tasks.erase(i);
        }
    }

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
    void pollTasks()
    {
        for (auto i = _uploaders.begin(); i != _uploaders.end();)
        {
            auto& uploader = i->second;
            uploader.removeFinishedTasks();
            if (!uploader.hasTasks())
            {
                i = _uploaders.erase(i);
                continue;
            }
            ++i;
        }
    }

    /**
     * @brief Register a new running model upload task.
     *
     * @param handle Client handle.
     * @param task Task to manage.
     */
    void addTask(const ConnectionHandle& handle, ModelUploadTaskPtr task)
    {
        _uploaders[handle].addTask(std::move(task));
    }

    /**
     * @brief Indicate that the next binary request from the given client will
     * be a chunk of the given model.
     *
     * @param handle Client handle.
     * @param id Model chunks ID given at model upload request.
     */
    void setNextChunkId(const ConnectionHandle& handle, const std::string& id)
    {
        auto i = _uploaders.find(handle);
        if (i == _uploaders.end())
        {
            throw EntrypointException("No model uploads are running");
        }
        auto& uploader = i->second;
        uploader.setNextChunkId(id);
    }

    /**
     * @brief Add the binary blob to the correct running mode upload task.
     *
     * @param handle Client handle.
     * @param packet Client binary message.
     */
    void processBinaryRequest(const ConnectionHandle& handle,
                              const InputPacket& packet)
    {
        auto i = _uploaders.find(handle);
        if (i == _uploaders.end())
        {
            Log::error("Processing binary from unknown client.");
            return;
        }
        auto& uploader = i->second;
        auto& data = packet.getData();
        uploader.addBlob(data);
    }

private:
    std::unordered_map<ConnectionHandle, ModelUploader> _uploaders;
};
} // namespace brayns