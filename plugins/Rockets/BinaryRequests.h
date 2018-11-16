/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
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

#include <brayns/common/log.h>
#include <brayns/tasks/AddModelFromBlobTask.h>
#include <brayns/tasks/errors.h>

#include <rockets/jsonrpc/types.h>

#include "jsonPropertyMap.h"

namespace brayns
{
const std::string METHOD_REQUEST_MODEL_UPLOAD = "request-model-upload";

/**
 * Manage requests for the request-model-upload RPC by receiving and delegating
 * the blobs to the correct request.
 */
class BinaryRequests
{
public:
    /**
     * Create and remember the AddModelFromBlobTask for upcoming receives of
     * binary data to delegate them to the task.
     */
    auto createTask(const BinaryParam& param, uintptr_t clientID,
                    Engine& engine)
    {
        auto task = std::make_shared<AddModelFromBlobTask>(param, engine);

        std::lock_guard<std::mutex> lock(_mutex);
        _requests.emplace(std::make_pair(clientID, param.chunksID), task);
        _nextChunkID = param.chunksID;

        return task;
    }

    void setNextChunkID(const std::string& id) { _nextChunkID = id; }
    /** The receive and delegate of blobs to the AddModelFromBlobTask. */
    rockets::ws::Response processMessage(const rockets::ws::Request& wsRequest)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        const auto key = std::make_pair(wsRequest.clientID, _nextChunkID);
        if (_requests.count(key) == 0)
        {
            BRAYNS_ERROR << "Missing RPC " << METHOD_REQUEST_MODEL_UPLOAD
                         << " or cancelled?" << std::endl;
            return {};
        }

        _requests[key]->appendBlob(wsRequest.message);
        return {};
    }

    /** Remove pending request in case the client connection closed. */
    void removeRequest(const uintptr_t clientID)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        for (auto i = _requests.begin(); i != _requests.end();)
        {
            if (i->first.first != clientID)
            {
                ++i;
                continue;
            }
            i->second->cancel();
            i = _requests.erase(i);
        }
    }

    /** Remove finished task. */
    void removeTask(TaskPtr task)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        for (auto i = _requests.begin(); i != _requests.end();)
        {
            if (i->second != task)
            {
                ++i;
                continue;
            }
            i = _requests.erase(i);
        }
    }

private:
    using ClientRequestID = std::pair<uintptr_t, std::string>;
    std::map<ClientRequestID, std::shared_ptr<AddModelFromBlobTask>> _requests;
    std::string _nextChunkID;
    std::mutex _mutex;
};
}
