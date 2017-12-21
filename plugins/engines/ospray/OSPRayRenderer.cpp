/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#include <brayns/common/log.h>

#include "OSPRayCamera.h"
#include "OSPRayFrameBuffer.h"
#include "OSPRayRenderer.h"
#include "OSPRayScene.h"

namespace brayns
{
OSPRayRenderer::OSPRayRenderer(const std::string& name,
                               ParametersManager& parametersManager)
    : Renderer(parametersManager)
    , _name(name)
    , _camera(0)
{
    _renderer = ospNewRenderer(name.c_str());
    assert(_renderer);
}

OSPRayRenderer::~OSPRayRenderer()
{
    ospRelease(_renderer);
}

void OSPRayRenderer::render(FrameBufferPtr frameBuffer)
{
    OSPRayFrameBuffer* osprayFrameBuffer =
        dynamic_cast<OSPRayFrameBuffer*>(frameBuffer.get());
    const auto variance =
        ospRenderFrame(osprayFrameBuffer->impl(), _renderer,
                       OSP_FB_COLOR | OSP_FB_DEPTH | OSP_FB_ACCUM);

    if (!frameBuffer->getAccumulation())
        return;

    if (variance == std::numeric_limits<float>::infinity())
        _hasNewImage = true;
    else
        _hasNewImage = std::abs(_prevVariance - variance) > 0.01;
    _prevVariance = variance;
}

void OSPRayRenderer::commit()
{
    AnimationParameters& ap = _parametersManager.getAnimationParameters();
    RenderingParameters& rp = _parametersManager.getRenderingParameters();
    SceneParameters& sp = _parametersManager.getSceneParameters();
    VolumeParameters& vp = _parametersManager.getVolumeParameters();

    if (!ap.getModified() && !rp.getModified() && !sp.getModified() &&
        !vp.getModified() && !_scene->getModified())
    {
        return;
    }

    ShadingType mt = rp.getShading();

    Vector3f color = rp.getBackgroundColor();
    ospSet3f(_renderer, "bgColor", color.x(), color.y(), color.z());
    ospSet1i(_renderer, "shadowsEnabled", rp.getShadows() > 0.f);
    ospSet1f(_renderer, "shadows", rp.getShadows());
    ospSet1f(_renderer, "softShadows", rp.getSoftShadows());
    ospSet1f(_renderer, "aoWeight", rp.getAmbientOcclusionStrength());
    ospSet1i(_renderer, "aoSamples", 1);
    ospSet1f(_renderer, "aoDistance", rp.getAmbientOcclusionDistance());
    ospSet1f(_renderer, "varianceThreshold", rp.getVarianceThreshold());

    ospSet1i(_renderer, "shadingEnabled", (mt == ShadingType::diffuse));
    ospSet1f(_renderer, "timestamp", ap.getFrame());
    ospSet1i(_renderer, "randomNumber", rand() % 10000);
    ospSet1i(_renderer, "spp", rp.getSamplesPerPixel());
    ospSet1i(_renderer, "electronShading", (mt == ShadingType::electron));
    ospSet1f(_renderer, "epsilon", rp.getEpsilon());
    ospSet1f(_renderer, "detectionDistance", rp.getDetectionDistance());
    ospSet1i(_renderer, "detectionOnDifferentMaterial",
             rp.getDetectionOnDifferentMaterial());
    color = rp.getDetectionNearColor();
    ospSet3f(_renderer, "detectionNearColor", color.x(), color.y(), color.z());
    color = rp.getDetectionFarColor();
    ospSet3f(_renderer, "detectionFarColor", color.x(), color.y(), color.z());
    ospSet1i(_renderer, "materialForSimulation",
             static_cast<size_t>(MaterialType::voltage_simulation));
    ospSet1i(_renderer, "volumeSamplesPerRay", vp.getSamplesPerRay());

    OSPRayScene* osprayScene = static_cast<OSPRayScene*>(_scene.get());
    assert(osprayScene);

    ospSetObject(_renderer, "world", osprayScene->modelImpl());
    ospSetObject(_renderer, "simulationModel",
                 osprayScene->simulationModelImpl());
    ospCommit(_renderer);
}

void OSPRayRenderer::setCamera(CameraPtr camera)
{
    _camera = static_cast<OSPRayCamera*>(camera.get());
    assert(_camera);
    ospSetObject(_renderer, "camera", _camera->impl());
    ospCommit(_renderer);
}

Renderer::PickResult OSPRayRenderer::pick(const Vector2f& pickPos)
{
    OSPPickResult ospResult;
    osp::vec2f pos{pickPos.x(), pickPos.y()};

    // HACK: as the time for picking is set to 0.5 and interpolated in a
    // (default) 0..0 range, the ray.time will be 0. So all geometries that have
    // a time > 0 (like branches that have distance to the soma for the growing
    // use-case), cannot be picked. So we make the range as large as possible to
    // make ray.time be as large as possible.
    ospSet1f(_camera->impl(), "shutterClose", INFINITY);
    ospCommit(_camera->impl());

    ospPick(&ospResult, _renderer, pos);

    // UNDO HACK
    ospSet1f(_camera->impl(), "shutterClose", 0.f);
    ospCommit(_camera->impl());

    PickResult result;
    result.hit = ospResult.hit;
    result.pos = {ospResult.position.x, ospResult.position.y,
                  ospResult.position.z};
    return result;
}
}
