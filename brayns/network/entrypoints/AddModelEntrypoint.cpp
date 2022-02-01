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

#include "AddModelEntrypoint.h"

#include <brayns/network/entrypoint/EntrypointException.h>
#include <brayns/network/entrypoint/EntrypointTask.h>

namespace
{
class LoadModelTask : public brayns::EntrypointTask<brayns::ModelParams, std::vector<brayns::ModelDescriptorPtr>>
{
public:
    LoadModelTask(brayns::Engine &engine, brayns::LoaderRegistry &registry)
        : _engine(&engine)
        , _loaderRegistry(&registry)
    {
    }

    virtual void run() override
    {
        auto &scene = _engine->getScene();
        auto &path = _params.getPath();
        auto &loaderName = _params.getLoaderName();

        const auto &loader = _loaderRegistry->getSuitableLoader(path, "", loaderName);

        _descriptors = loader.loadFromFile(
            path,
            {[this](const auto &operation, auto amount) { progress(operation, amount); }},
            _params.getLoadParameters(),
            scene);

        scene.addModels(_descriptors, _params);
    }

    virtual void onStart() override
    {
        _params = getParams();
        auto &path = _params.getPath();
        if (path.empty())
        {
            throw brayns::EntrypointException("Missing model path");
        }
        if (!_loaderRegistry->isSupportedFile(path))
        {
            throw brayns::EntrypointException("Unsupported file type: '" + path + "'");
        }
    }

    virtual void onComplete() override
    {
        _engine->triggerRender();
        reply(_descriptors);
    }

private:
    brayns::Engine *_engine;
    brayns::LoaderRegistry *_loaderRegistry;
    brayns::ModelParams _params;
    std::vector<brayns::ModelDescriptorPtr> _descriptors;
};
} // namespace

namespace brayns
{
std::string AddModelEntrypoint::getName() const
{
    return "add-model";
}

std::string AddModelEntrypoint::getDescription() const
{
    return "Add model from path and return model descriptor on success";
}

bool AddModelEntrypoint::isAsync() const
{
    return true;
}

void AddModelEntrypoint::onRequest(const Request &request)
{
    auto &api = getApi();
    auto &engine = api.getEngine();
    auto &registry = api.getLoaderRegistry();
    auto task = std::make_shared<LoadModelTask>(engine, registry);
    launchTask(task, request);
}
} // namespace brayns
