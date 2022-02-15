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

#include <brayns/network/entrypoint/EntrypointTask.h>
#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace
{
class ModelParametersValidator
{
public:
    static void validate(const brayns::ModelParams &params, const brayns::LoaderRegistry &loaders)
    {
        auto &path = params.getPath();
        if (path.empty())
        {
            throw brayns::JsonRpcException("Missing model path");
        }
        if (!loaders.isSupportedFile(path))
        {
            throw brayns::JsonRpcException("Unsupported file type: '" + path + "'");
        }
    }
};

class LoadModelTask : public brayns::EntrypointTask<brayns::ModelParams, std::vector<brayns::ModelDescriptorPtr>>
{
public:
    LoadModelTask(Request request, brayns::Scene &scene, const brayns::LoaderRegistry &loaders)
        : EntrypointTask(std::move(request))
        , _scene(scene)
        , _loaders(loaders)
    {
        _params = getParams();
        ModelParametersValidator::validate(_params, _loaders);
    }

    virtual void run() override
    {
        auto &path = _params.getPath();
        auto &name = _params.getLoaderName();
        auto &loader = _loaders.getSuitableLoader(path, "", name);
        auto &parameters = _params.getLoadParameters();
        auto callback = [this](const auto &operation, auto amount) { progress(operation, amount); };
        _descriptors = loader.loadFromFile(path, {callback}, parameters, _scene);
        _scene.addModels(_descriptors, _params);
    }

    virtual void onComplete() override
    {
        reply(_descriptors);
    }

private:
    brayns::Scene &_scene;
    const brayns::LoaderRegistry &_loaders;
    brayns::ModelParams _params;
    std::vector<brayns::ModelDescriptorPtr> _descriptors;
};
} // namespace

namespace brayns
{
AddModelEntrypoint::AddModelEntrypoint(Scene &scene, LoaderRegistry &loaders, INetworkInterface &interface)
    : _scene(scene)
    , _loaders(loaders)
    , _launcher(interface)
{
}

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
    auto task = std::make_unique<LoadModelTask>(request, _scene, _loaders);
    _launcher.launch(std::move(task));
}
} // namespace brayns
