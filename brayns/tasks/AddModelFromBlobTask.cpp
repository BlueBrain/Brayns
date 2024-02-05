/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include "AddModelFromBlobTask.h"

#include "Errors.h"
#include "LoadModelFunctor.h"

#include <sstream>

namespace brayns
{
AddModelFromBlobTask::AddModelFromBlobTask(const BinaryParam& param,
                                           Engine& engine,
                                           LoaderRegistry& registry)
    : _param(param)
{
    _checkValidity(registry);

    _blob.reserve(param.size);

    LoadModelFunctor functor{engine, registry, param};
    functor.setCancelToken(_cancelToken);
    functor.setProgressFunc(
        [& progress = progress, w = CHUNK_PROGRESS_WEIGHT](const auto& msg,
                                                           auto, auto amount) {
            progress.update(msg, w + (amount * (1.f - w)));
        });

    // load data, return model descriptor or stop if blob receive was invalid
    _finishTasks.emplace_back(_errorEvent.get_task());
    _finishTasks.emplace_back(_chunkEvent.get_task().then(std::move(functor)));
    _task =
        async::when_any(_finishTasks)
            .then([&engine](async::when_any_result<std::vector<
                                async::task<std::vector<ModelDescriptorPtr>>>>
                                results) {
                engine.triggerRender();
                return results.tasks[results.index].get();
            });
}

void AddModelFromBlobTask::appendBlob(const std::string& blob)
{
    // if more bytes than expected are received, error and stop
    if (_blob.size() + blob.size() > _param.size)
    {
        _errorEvent.set_exception(
            std::make_exception_ptr(INVALID_BINARY_RECEIVE));
        return;
    }

    _blob.insert(_blob.end(), blob.begin(), blob.end());

    _receivedBytes += blob.size();
    std::stringstream msg;
    msg << "Receiving " << _param.getName() << " ...";
    progress.update(msg.str(), _progressBytes());

    // if blob is complete, start the loading
    if (_blob.size() == _param.size)
        _chunkEvent.set({_param.type, _param.getName(), std::move(_blob)});
}

void AddModelFromBlobTask::_checkValidity(LoaderRegistry& registry)
{
    if (_param.type.empty() || _param.size == 0)
        throw MISSING_PARAMS;

    if (!registry.isSupportedType(_param.type))
        throw UNSUPPORTED_TYPE;
}
} // namespace brayns
