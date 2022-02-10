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

#include "ModelUploadManager.h"

#include <cassert>

namespace brayns
{
InvalidChunkRequestException::InvalidChunkRequestException(const ClientRef &client)
    : InvalidRequestException("No model uploads for client " + std::to_string(client.getId()))
{
}

InvalidChunkRequestException::InvalidChunkRequestException(const ClientRef &client, const std::string &chunksId)
    : InvalidRequestException(
        "No model uploads for client " + std::to_string(client.getId()) + " and chunks ID " + chunksId)
{
}

void ModelUploadManager::poll()
{
    for (auto i = _clientUploads.begin(); i != _clientUploads.end();)
    {
        auto &clientUpload = i->second;
        auto &tasks = clientUpload.tasks;
        for (auto j = tasks.begin(); j != tasks.end();)
        {
            auto &task = *j->second;
            if (task.poll())
            {
                j = tasks.erase(j);
                continue;
            }
            ++j;
        }
        if (tasks.empty())
        {
            i = _clientUploads.erase(i);
            continue;
        }
        ++i;
    }
}

void ModelUploadManager::add(const ClientRef &client, std::unique_ptr<ModelUploadTask> task)
{
    assert(task);
    auto &clientUpload = _clientUploads[client];
    auto &tasks = clientUpload.tasks;
    auto &chunksId = task->getChunksId();
    auto &currentTask = tasks[chunksId];
    if (currentTask)
    {
        throw InvalidRequestException("A model upload with chunks ID '" + chunksId + "' is already running");
    }
    clientUpload.nextChunkId = chunksId;
    currentTask = std::move(task);
}

void ModelUploadManager::setNextChunkId(const ClientRef &client, const std::string &id)
{
    auto i = _clientUploads.find(client);
    if (i == _clientUploads.end())
    {
        throw InvalidRequestException("No model uploads running.");
    }
    auto &clientUpload = i->second;
    clientUpload.nextChunkId = id;
}

void ModelUploadManager::addChunk(const ClientRef &client, std::string_view data)
{
    auto i = _clientUploads.find(client);
    if (i == _clientUploads.end())
    {
        throw InvalidChunkRequestException(client);
    }
    auto &clientUpload = i->second;
    auto &chunksId = clientUpload.nextChunkId;
    auto &tasks = clientUpload.tasks;
    auto j = tasks.find(chunksId);
    if (j == tasks.end())
    {
        throw InvalidChunkRequestException(client, chunksId);
    }
    auto &task = *j->second;
    task.addChunk(data);
}
} // namespace brayns
