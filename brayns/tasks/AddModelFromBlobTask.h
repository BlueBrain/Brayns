/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include <brayns/common/loader/Loader.h>
#include <brayns/common/tasks/Task.h>
#include <brayns/engine/Model.h>

namespace brayns
{
struct BinaryParam;
}
SERIALIZATION_ACCESS(BinaryParam)

namespace brayns
{
struct Chunk
{
    std::string id;
};

struct BinaryParam : ModelParams
{
    size_t size{0};   //!< size in bytes of file
    std::string type; //!< file extension or type (MESH, POINTS, CIRCUIT)
    std::string chunksID;
    SERIALIZATION_FRIEND(BinaryParam)
};

/**
 * A task which receives a file blob, triggers loading of the received blob
 * and adds the loaded model to the engines' scene.
 */
class AddModelFromBlobTask : public Task<ModelDescriptorPtr>
{
public:
    AddModelFromBlobTask(const BinaryParam& param, Engine& engine);

    void appendBlob(const std::string& blob);

private:
    void _checkValidity(Engine& engine);
    void _cancel() final
    {
        _chunkEvent.set_exception(
            std::make_exception_ptr(async::task_canceled()));
    }
    float _progressBytes() const
    {
        return CHUNK_PROGRESS_WEIGHT * ((float)_receivedBytes / _param.size);
    }

    async::event_task<Blob> _chunkEvent;
    async::event_task<ModelDescriptorPtr> _errorEvent;
    std::vector<async::task<ModelDescriptorPtr>> _finishTasks;
    std::string _blob;
    BinaryParam _param;
    size_t _receivedBytes{0};
    const float CHUNK_PROGRESS_WEIGHT{0.5f};
};
}
