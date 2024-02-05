/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <brayns/common/Log.h>
#include <brayns/common/scene/ClipPlane.h>
#include <brayns/engine/Model.h>

#include "OSPRayCamera.h"
#include "OSPRayFrameBuffer.h"
#include "OSPRayMaterial.h"
#include "OSPRayModel.h"
#include "OSPRayRenderer.h"
#include "OSPRayScene.h"
#include "utils.h"

namespace brayns
{
OSPRayRenderer::OSPRayRenderer(const AnimationParameters& animationParameters,
                               const RenderingParameters& renderingParameters)
    : Renderer(animationParameters, renderingParameters)
{
}

OSPRayRenderer::~OSPRayRenderer()
{
    _destroyRenderer();
}

void OSPRayRenderer::_destroyRenderer()
{
    if (_renderer)
        ospRelease(_renderer);
    _renderer = nullptr;
}

void OSPRayRenderer::render(FrameBufferPtr frameBuffer)
{
    auto osprayFrameBuffer =
        std::static_pointer_cast<OSPRayFrameBuffer>(frameBuffer);
    auto lock = osprayFrameBuffer->getScopeLock();

    _variance = ospRenderFrame(osprayFrameBuffer->impl(), _renderer,
                               OSP_FB_COLOR | OSP_FB_DEPTH | OSP_FB_ACCUM);

    osprayFrameBuffer->markModified();
}

void OSPRayRenderer::commit()
{
    const AnimationParameters& ap = _animationParameters;
    const RenderingParameters& rp = _renderingParameters;
    auto scene = std::static_pointer_cast<OSPRayScene>(_scene);
    const bool lightsChanged = _currLightsData != scene->lightData();
    const bool rendererChanged = _currentOSPRenderer != getCurrentType();

    if (!ap.isModified() && !rp.isModified() && !_scene->isModified() &&
        !isModified() && !_camera->isModified() && !lightsChanged &&
        !rendererChanged)
    {
        return;
    }

    if (rendererChanged)
        _createOSPRenderer();

    toOSPRayProperties(*this, _renderer);

    if (lightsChanged || rendererChanged)
    {
        ospSetData(_renderer, "lights", scene->lightData());
        _currLightsData = scene->lightData();
    }

    if (isModified() || rendererChanged || _scene->isModified())
    {
        _commitRendererMaterials();

        auto simulationData = scene->getSimulationData();
        auto transferFunc = scene->getTransferFunctionImpl();
        if (simulationData != nullptr && transferFunc != nullptr)
        {
            ospSetData(_renderer, "simulationData", simulationData);
            ospSetObject(_renderer, "transferFunction", transferFunc);
            ospSetObject(_renderer, "secondaryModel", nullptr);
        }
        else
        {
            // ospRemoveParam leaks objects, so we set it to null first
            ospSetData(_renderer, "simulationData", nullptr);
            ospRemoveParam(_renderer, "simulationData");
        }

        // Setting the clip planes in the renderer and the camera
        std::vector<Plane> planes;
        for (const auto& clipPlane : _scene->getClipPlanes())
            planes.push_back(clipPlane->getPlane());

        setClipPlanes(planes);

        _camera->setClipPlanes(planes);
        _camera->commit();
    }

    osphelper::set(_renderer, "timestamp", static_cast<float>(ap.getFrame()));
    osphelper::set(_renderer, "randomNumber", 1);
    osphelper::set(_renderer, "bgColor", Vector3f(rp.getBackgroundColor()));
    osphelper::set(_renderer, "varianceThreshold",
                   static_cast<float>(rp.getVarianceThreshold()));
    osphelper::set(_renderer, "spp", static_cast<int>(rp.getSamplesPerPixel()));

    if (auto material = std::static_pointer_cast<OSPRayMaterial>(
            scene->getBackgroundMaterial()))
    {
        material->setDiffuseColor(rp.getBackgroundColor());
        material->commit(_currentOSPRenderer);
        ospSetObject(_renderer, "bgMaterial", material->getOSPMaterial());
    }

    // Clip planes
    if (!_clipPlanes.empty())
    {
        const auto clipPlanes = convertVectorToFloat(_clipPlanes);
        auto clipPlaneData =
            ospNewData(clipPlanes.size(), OSP_FLOAT4, clipPlanes.data());
        ospSetData(_renderer, "clipPlanes", clipPlaneData);
        ospRelease(clipPlaneData);
    }
    else
    {
        // ospRemoveParam leaks objects, so we set it to null first
        ospSetData(_renderer, "clipPlanes", nullptr);
        ospRemoveParam(_renderer, "clipPlanes");
    }

    ospSetObject(_renderer, "camera", _camera->impl());
    ospSetObject(_renderer, "world", scene->getModel());
    ospCommit(_renderer);
}

void OSPRayRenderer::setCamera(CameraPtr camera)
{
    _camera = static_cast<OSPRayCamera*>(camera.get());
    assert(_camera);
    if (_renderer)
        ospSetObject(_renderer, "camera", _camera->impl());
    markModified();
}

Renderer::PickResult OSPRayRenderer::pick(const Vector2f& pickPos)
{
    OSPPickResult ospResult;
    osp::vec2f pos{pickPos.x, pickPos.y};

    // HACK: as the time for picking is set to 0.5 and interpolated in a
    // (default) 0..0 range, the ray.time will be 0. So all geometries that have
    // a time > 0 (like branches that have distance to the soma for the growing
    // use-case), cannot be picked. So we make the range as large as possible to
    // make ray.time be as large as possible.
    osphelper::set(_camera->impl(), "shutterClose", INFINITY);
    ospCommit(_camera->impl());

    ospPick(&ospResult, _renderer, pos);

    // UNDO HACK
    osphelper::set(_camera->impl(), "shutterClose", 0.f);
    ospCommit(_camera->impl());

    PickResult result;
    result.hit = ospResult.hit;
    if (result.hit)
        result.pos = {ospResult.position.x, ospResult.position.y,
                      ospResult.position.z};
    return result;
}

void OSPRayRenderer::_createOSPRenderer()
{
    auto newRenderer = ospNewRenderer(getCurrentType().c_str());
    if (!newRenderer)
        throw std::runtime_error(getCurrentType() +
                                 " is not a registered renderer");
    _destroyRenderer();
    _renderer = newRenderer;
    if (_camera)
        ospSetObject(_renderer, "camera", _camera->impl());
    _currentOSPRenderer = getCurrentType();
    markModified(false);
}

void OSPRayRenderer::_commitRendererMaterials()
{
    _scene->visitModels([& renderer = _currentOSPRenderer](Model& model) {
        static_cast<OSPRayModel&>(model).commitMaterials(renderer);
    });
}

void OSPRayRenderer::setClipPlanes(const std::vector<Plane>& planes)
{
    if (_clipPlanes == planes)
        return;
    _clipPlanes = planes;
    markModified(false);
}

} // namespace brayns
