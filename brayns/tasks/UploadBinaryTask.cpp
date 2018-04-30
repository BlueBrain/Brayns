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

#include "UploadBinaryTask.h"

#include "LoadDataFunctor.h"
#include "errors.h"

#include <brayns/common/engine/Engine.h>
#include <brayns/common/scene/Scene.h>

namespace brayns
{
UploadBinaryTask::UploadBinaryTask(const BinaryParams& params, EnginePtr engine)
    : _params(params)
{
    if (params.empty())
        throw MISSING_PARAMS;

    _checkValidity(engine);

    _chunks.resize(params.size());

    // create load tasks for each param
    for (size_t i = 0; i < params.size(); ++i)
    {
        LoadDataFunctor functor{engine};
        functor.setCancelToken(_cancelToken);

        // use progress increment as we might receive data for next file which
        // updates progress as well
        functor.setProgressFunc([&progress=progress,
                      amountPerTask = (1.f-CHUNK_PROGRESS_WEIGHT)/params.size()]
        (auto msg, auto increment, auto){
            progress.increment(msg, increment*amountPerTask);
        });

        // chunk event task is set() from appendBlob() once all data has been
        // received, then loading starts.
        _loadTasks.push_back(_chunks[i].get_task().then(std::move(functor)));
    }

    // create a task to wait for loading of all files to be finished
    auto allLoaded =
        async::when_all(_loadTasks)
            .then([](std::vector<async::task<void>> results) {
                for (auto& result : results)
                    result.get(); // exception is propagated to caller
            });

    // either finish with success/error from loading, or cancel
    _finishTasks.emplace_back(_errorEvent.get_task());
    _finishTasks.emplace_back(std::move(allLoaded));
    _task = async::when_any(_finishTasks)
                .then([](async::when_any_result<std::vector<async::task<void>>>
                             results) {
                    // exception is propagated to caller
                    results.tasks[results.index].get();
                    return true;
                });
}

void UploadBinaryTask::appendBlob(const std::string& blob)
{
    // if more blobs or bytes than expected are received, error and stop
    if (_index >= _params.size() ||
        (_blob.size() + blob.size() > _params[_index].size))
    {
        _errorEvent.set_exception(
            std::make_exception_ptr(INVALID_BINARY_RECEIVE));
        return;
    }

    _blob += blob;

    // update progress; use increment as we might load at the same time
    const auto before = _progressBytes();
    _receivedBytes += blob.size();
    std::stringstream msg;
    msg << "Receiving " << _params[_index].name << " ...";
    progress.increment(msg.str(), _progressBytes() - before);

    // if this blob is complete, start the loading
    if (_blob.size() == _params[_index].size)
    {
        _chunks[_index].set(
            {_params[_index].type, _params[_index].name, std::move(_blob)});

        // prepare next blob receive
        ++_index;
        if (_index < _params.size())
            _blob.reserve(_params[_index].size);
    }
}

void UploadBinaryTask::_checkValidity(EnginePtr engine)
{
    const auto& registry = engine->getScene().getLoaderRegistry();

    // pre-check for validity of given params
    for (size_t i = 0; i < _params.size(); ++i)
    {
        const auto& param = _params[i];
        if (param.type.empty() || param.size == 0)
            throw MISSING_PARAMS;

        if (!registry.isSupported(param.type))
        {
            const auto& types = registry.supportedTypes();
            throw UNSUPPORTED_TYPE({i, {types.begin(), types.end()}});
        }

        _totalBytes += param.size;
    }
}
}
