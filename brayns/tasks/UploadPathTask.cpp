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

#include "UploadPathTask.h"

#include "LoadDataFunctor.h"
#include "errors.h"

#include <brayns/common/engine/Engine.h>
#include <brayns/common/scene/Scene.h>

namespace brayns
{
UploadPathTask::UploadPathTask(const std::vector<std::string>& paths,
                               EnginePtr engine)
{
    if (paths.empty())
        throw MISSING_PARAMS;

    const auto& registry = engine->getScene().getLoaderRegistry();
    const auto& supportedTypes = registry.supportedTypes();

    // pre-check for validity of given paths
    for (size_t i = 0; i < paths.size(); ++i)
    {
        const auto& path = paths[i];
        if (!registry.isSupported(path))
            throw UNSUPPORTED_TYPE(
                {i, {supportedTypes.begin(), supportedTypes.end()}});
    }

    // create and start load tasks
    for (const auto& path : paths)
    {
        LoadDataFunctor functor{engine};
        functor.setCancelToken(_cancelToken);

        functor.setProgressFunc(
            [& progress = progress,
             amountPerTask = 1.f / paths.size() ](auto msg, auto increment,
                                                  auto) {
                progress.increment(msg, increment * amountPerTask);
            });
        _loadTasks.push_back(
            async::spawn([path] { return path; }).then(std::move(functor)));
    }

    // wait for loading of all paths to be finished
    _task = async::when_all(_loadTasks)
                .then([](std::vector<async::task<void>> results) {
                    for (auto& result : results)
                        result.get(); // exception is propagated to caller
                    return true;
                });
}
}
