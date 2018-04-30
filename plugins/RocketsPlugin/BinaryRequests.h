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

#include <brayns/common/log.h>
#include <brayns/tasks/UploadBinaryTask.h>
#include <brayns/tasks/errors.h>

#include <rockets/jsonrpc/types.h>

namespace brayns
{
const std::string METHOD_UPLOAD_BINARY = "upload-binary";

/**
 * Manage requests for the upload-binary RPC by receiving and delegating the
 * blobs to the correct request.
 */
class BinaryRequests
{
public:
    /**
     * Create and remember the UploadBinaryTask for upcoming receives of binary
     * data to delegate them to the task.
     *
     * Only one upload per client at a time is permitted.
     */
    auto createTask(const BinaryParams& params, uintptr_t clientID,
                    EnginePtr engine)
    {
        if (_binaryRequests.count(clientID) != 0)
            throw ALREADY_PENDING_REQUEST;

        auto task = std::make_shared<UploadBinaryTask>(params, engine);
        _binaryRequests.emplace(clientID, task);
        _requests.emplace(task, clientID);

        return task;
    }

    /** The receive and delegate of blobs to the UploadBinaryTask. */
    rockets::ws::Response processMessage(const rockets::ws::Request& wsRequest)
    {
        if (_binaryRequests.count(wsRequest.clientID) == 0)
        {
            BRAYNS_ERROR << "Missing RPC " << METHOD_UPLOAD_BINARY
                         << " or cancelled?" << std::endl;
            return {};
        }

        _binaryRequests[wsRequest.clientID]->appendBlob(wsRequest.message);
        return {};
    }

    /** Remove pending request in case the client connection closed. */
    void removeRequest(const uintptr_t clientID)
    {
        auto i = _binaryRequests.find(clientID);
        if (i == _binaryRequests.end())
            return;

        i->second->cancel();
        _binaryRequests.erase(i);
    }

    /** Remove finished task. */
    void removeTask(TaskPtr task)
    {
        auto i = _requests.find(task);
        if (i == _requests.end())
            return;

        removeRequest(i->second);
        _requests.erase(i);
    }

private:
    std::map<uintptr_t, std::shared_ptr<UploadBinaryTask>> _binaryRequests;
    std::map<TaskPtr, uintptr_t> _requests;
};
}
