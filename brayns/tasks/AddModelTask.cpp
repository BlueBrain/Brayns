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

#include "AddModelTask.h"

#include "LoadModelFunctor.h"
#include "errors.h"

#include <brayns/common/engine/Engine.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>

namespace brayns
{
AddModelTask::AddModelTask(const ModelParams& modelParams, EnginePtr engine)
{
    const auto& registry = engine->getScene().getLoaderRegistry();

    // pre-check for validity of given paths
    const auto& path = modelParams.getPath();
    if (path.empty())
        throw MISSING_PARAMS;

    if (!registry.isSupported(path))
    {
        const auto& supportedTypes = registry.supportedTypes();
        throw UNSUPPORTED_TYPE(
            {{supportedTypes.begin(), supportedTypes.end()}});
    }

    LoadModelFunctor functor{engine};
    functor.setCancelToken(_cancelToken);
    functor.setProgressFunc([& progress = progress](const auto& msg, auto,
                                                    auto amount) {
        progress.update(msg, amount);
    });

    // load data, trigger rendering, return model descriptor
    _task = async::spawn([path = modelParams.getPath()] { return path; })
                .then(std::move(functor))
                .then([engine,
                       modelParams](async::task<ModelDescriptorPtr> result) {
                    auto modelDescriptor = result.get();
                    if (modelDescriptor)
                        *modelDescriptor = modelParams;
                    engine->triggerRender();
                    return modelDescriptor;
                });
}
}
