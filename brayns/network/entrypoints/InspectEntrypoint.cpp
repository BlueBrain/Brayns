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

#include "InspectEntrypoint.h"

namespace
{
class SceneInspector
{
public:
    SceneInspector(brayns::Engine &engine)
        : _engine(engine)
    {
    }

    brayns::InspectResult inspect(const brayns::Vector2f &position)
    {
        auto pickResult = _pickOSPRayScene(position);
        if (!pickResult.hasHit)
        {
            brayns::InspectResult result;
            result.hit = false;
            return result;
        }

        auto &instance = _findHittedModel(pickResult.instance);
        const auto inspectContext = _buildInspectContext(pickResult);
        auto metadata = brayns::JsonObject();

        auto &model = instance.getModel();
        model.onInspect(inspectContext, metadata);

        _releaseOSPRayHandles(pickResult);

        return _buildResult(inspectContext, instance, metadata);
    }

private:
    OSPPickResult _pickOSPRayScene(const brayns::Vector2f &position)
    {
        auto &frameBuffer = _engine.getFrameBuffer();
        auto &renderer = _engine.getRenderer();
        auto &camera = _engine.getCamera();
        auto &scene = _engine.getScene();

        auto frameBufferHandle = frameBuffer.handle();
        auto rendererHandle = renderer.handle();
        auto cameraHandle = camera.handle();
        auto sceneHandle = scene.handle();

        auto x = position.x;
        auto y = position.y;

        OSPPickResult result;
        ospPick(&result, frameBufferHandle, rendererHandle, cameraHandle, sceneHandle, x, y);
        return result;
    }

    brayns::ModelInstance &_findHittedModel(OSPInstance hittedInstance)
    {
        auto &scene = _engine.getScene();
        auto &models = scene.getModels();
        auto &instances = models.getAllModelInstances();

        auto begin = instances.begin();
        auto end = instances.end();
        auto instanceIterator = std::find_if(
            begin,
            end,
            [&](brayns::ModelInstance *instancePtr) { return instancePtr->handle() == hittedInstance; });

        // Shouldn't happen, but..
        if (instanceIterator == end)
        {
            throw brayns::InternalErrorException("Could not find the hitted model instance in the scene");
        }

        return **instanceIterator;
    }

    brayns::InspectContext _buildInspectContext(OSPPickResult &osprayPickResult)
    {
        const auto &ospHitPosition = osprayPickResult.worldPosition;
        auto hitPosition = brayns::Vector3f(ospHitPosition[0], ospHitPosition[1], ospHitPosition[2]);
        auto geometricModel = osprayPickResult.model;
        auto primitiveIndex = osprayPickResult.primID;

        return brayns::InspectContext{hitPosition, geometricModel, primitiveIndex};
    }

    brayns::InspectResult _buildResult(
        const brayns::InspectContext &context,
        const brayns::ModelInstance &instance,
        brayns::JsonObject &metadata)
    {
        brayns::InspectResult result;
        result.hit = true;
        result.model_id = instance.getID();
        result.position = context.hitPosition;
        result.metadata = metadata;
        return result;
    }

    void _releaseOSPRayHandles(OSPPickResult &osprayPickResult)
    {
        auto instanceHandle = osprayPickResult.instance;
        auto modelHandle = osprayPickResult.model;
        ospRelease(instanceHandle);
        ospRelease(modelHandle);
    }

private:
    brayns::Engine &_engine;
};
}

namespace brayns
{
InspectEntrypoint::InspectEntrypoint(Engine &engine)
    : _engine(engine)
{
}

std::string InspectEntrypoint::getMethod() const
{
    return "inspect";
}

std::string InspectEntrypoint::getDescription() const
{
    return "Inspect the scene at x-y position";
}

void InspectEntrypoint::onRequest(const Request &request)
{
    const auto params = request.getParams();
    const auto &position = params.position;

    SceneInspector inspector(_engine);
    auto result = inspector.inspect(position);
    request.reply(result);
}
} // namespace brayns
