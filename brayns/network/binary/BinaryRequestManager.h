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

#include <string>
#include <unordered_map>

#include <brayns/common/log.h>

#include <brayns/network/entrypoint/EntrypointException.h>
#include <brayns/network/socket/ConnectionHandle.h>

#include "ModelUploadTask.h"

namespace brayns
{
class ModelUploader
{
public:
    bool hasTasks() const { return !_tasks.empty(); }

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

    void setNextChunkId(const std::string& id) { _nextChunkId = id; }

    void addBlob(const std::string& blob)
    {
        auto i = _tasks.find(_nextChunkId);
        if (i == _tasks.end())
        {
            BRAYNS_ERROR << "No model upload with chunks ID '" << _nextChunkId
                         << "'.\n";
            return;
        }
        auto& task = *i->second;
        task.addBlob(blob);
    }

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
    void update()
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
            BRAYNS_ERROR << "Processing binary from unknown client.\n";
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