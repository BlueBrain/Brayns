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

#include "AddModelTask.h"

#include "Errors.h"
#include "LoadModelFunctor.h"

namespace brayns
{
AddModelTask::AddModelTask(const ModelParams& modelParams, Engine& engine,
                           LoaderRegistry& registry)
{
    // pre-check for validity of given paths
    const auto& path = modelParams.getPath();
    if (path.empty())
        throw MISSING_PARAMS;

    if (!registry.isSupportedFile(path))
        throw UNSUPPORTED_TYPE;

    LoadModelFunctor functor{engine, registry, modelParams};
    functor.setCancelToken(_cancelToken);
    functor.setProgressFunc(
        [& progress = progress](const auto& msg, auto, auto amount) {
            progress.update(msg, amount);
        });

    // load data, return model descriptor
    _task = async::spawn(std::move(functor))
                .then([&engine](
                          async::task<std::vector<ModelDescriptorPtr>> result) {
                    engine.triggerRender();
                    return result.get();
                });
}
} // namespace brayns
