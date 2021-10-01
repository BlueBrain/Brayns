/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#pragma once

#include <brayns/common/loader/Loader.h>

#include <brayns/engine/Scene.h>

#include <brayns/network/adapters/ModelParamsAdapter.h>
#include <brayns/network/entrypoint/Entrypoint.h>
#include <brayns/network/entrypoint/EntrypointTask.h>

namespace brayns
{
class LoadModelTask : public EntrypointTask<ModelParams, ModelDescriptors>
{
public:
    LoadModelTask(Engine& engine)
        : _engine(&engine)
    {
    }

    virtual void run() override
    {
        auto& scene = _engine->getScene();
        auto& path = _params.getPath();
        _descriptors =
            scene.loadModels(path, _params,
                             {[this](const auto& operation, auto amount) {
                                 progress(operation, amount);
                             }});
    }

    virtual void onStart() override
    {
        _params = getParams();
        auto& scene = _engine->getScene();
        auto& registry = scene.getLoaderRegistry();
        auto& path = _params.getPath();
        if (path.empty())
        {
            throw EntrypointException("Missing model path");
        }
        if (!registry.isSupportedFile(path))
        {
            throw EntrypointException("Unsupported file type: '" + path + "'");
        }
    }

    virtual void onComplete() override
    {
        _engine->triggerRender();
        reply(_descriptors);
    }

private:
    Engine* _engine;
    ModelParams _params;
    ModelDescriptors _descriptors;
};

class AddModelEntrypoint : public Entrypoint<ModelParams, ModelDescriptors>
{
public:
    virtual std::string getName() const override { return "add-model"; }

    virtual std::string getDescription() const override
    {
        return "Add model from path and return model descriptor on success";
    }

    virtual bool isAsync() const override { return true; }

    virtual void onRequest(const Request& request) override
    {
        auto& engine = getApi().getEngine();
        auto task = std::make_shared<LoadModelTask>(engine);
        launchTask(task, request);
    }
};
} // namespace brayns