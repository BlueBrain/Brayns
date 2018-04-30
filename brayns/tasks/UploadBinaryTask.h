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

#include <brayns/common/tasks/Task.h>

namespace brayns
{
struct BinaryParam
{
    size_t size{0};   //!< size in bytes of file
    std::string type; //!< file extension or type (MESH, POINTS, CIRCUIT)
    std::string name{"data"}; //!< name of the file
};

using BinaryParams = std::vector<BinaryParam>;

/**
 * A task which receives blobs of files, triggers loading for each received blob
 * and adds the loaded data to the scene.
 */
class UploadBinaryTask : public Task<bool>
{
public:
    UploadBinaryTask(const BinaryParams& params, EnginePtr engine);

    void appendBlob(const std::string& blob);

private:
    std::vector<async::task<void>> _loadTasks;
    std::vector<async::event_task<Blob>> _chunks;
    async::event_task<void> _errorEvent;
    std::vector<async::task<void>> _finishTasks;
    std::string _blob;
    size_t _index{0};

    void _checkValidity(EnginePtr engine);
    void _cancel() final
    {
        for (auto& i : _chunks)
            i.set_exception(std::make_exception_ptr(async::task_canceled()));
    }
    float _progressBytes() const
    {
        return CHUNK_PROGRESS_WEIGHT * ((float)_receivedBytes / _totalBytes);
    }

    BinaryParams _params;
    size_t _totalBytes{0};
    size_t _receivedBytes{0};
    const float CHUNK_PROGRESS_WEIGHT{0.5f};
};
}
