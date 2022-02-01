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

#include "BinaryRequestManager.h"

#include <brayns/common/Log.h>

#include <brayns/network/entrypoint/EntrypointException.h>

namespace brayns
{
bool ModelUploader::hasTasks() const
{
    return !_tasks.empty();
}

void ModelUploader::addTask(ModelUploadTaskPtr task)
{
    auto &chunksId = task->getChunksId();
    auto &currentTask = _tasks[chunksId];
    if (currentTask)
    {
        throw EntrypointException("A model upload with chunks ID '" + chunksId + "' is already running");
    }
    _nextChunkId = chunksId;
    currentTask = std::move(task);
}

void ModelUploader::setNextChunkId(const std::string &id)
{
    _nextChunkId = id;
}

void ModelUploader::addBlob(const std::string &blob)
{
    auto i = _tasks.find(_nextChunkId);
    if (i == _tasks.end())
    {
        Log::error("No model upload with chunks ID '{}'.", _nextChunkId);
        return;
    }
    auto &task = *i->second;
    task.addBlob(blob);
}

void ModelUploader::removeFinishedTasks()
{
    for (auto i = _tasks.begin(); i != _tasks.end();)
    {
        auto &task = *i->second;
        if (task.isRunning())
        {
            ++i;
            continue;
        }
        i = _tasks.erase(i);
    }
}

void BinaryRequestManager::pollTasks()
{
    for (auto i = _uploaders.begin(); i != _uploaders.end();)
    {
        auto &uploader = i->second;
        uploader.removeFinishedTasks();
        if (!uploader.hasTasks())
        {
            i = _uploaders.erase(i);
            continue;
        }
        ++i;
    }
}

void BinaryRequestManager::addTask(const ConnectionHandle &handle, ModelUploadTaskPtr task)
{
    _uploaders[handle].addTask(std::move(task));
}

void BinaryRequestManager::setNextChunkId(const ConnectionHandle &handle, const std::string &id)
{
    auto i = _uploaders.find(handle);
    if (i == _uploaders.end())
    {
        throw EntrypointException("No model uploads are running");
    }
    auto &uploader = i->second;
    uploader.setNextChunkId(id);
}

void BinaryRequestManager::processBinaryRequest(const ConnectionHandle &handle, const InputPacket &packet)
{
    auto i = _uploaders.find(handle);
    if (i == _uploaders.end())
    {
        Log::error("Processing binary from unknown client.");
        return;
    }
    auto &uploader = i->second;
    auto &data = packet.getData();
    uploader.addBlob(data);
}
} // namespace brayns